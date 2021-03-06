#include "bIdxThrPool.h"
#include "bIdxer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define BIDX_MAX_WORKER 16
#define BIDX_MAXEVENTS 64
typedef struct _bIdxServer
{
    int sfd;
    int efd;
    bIdxThrPool* pool;
    struct epoll_event *events;
}bIdxServer;

typedef  struct _bIdxServerArg
{
    int fd;
    bIdxer* idxer;
}bIdxServerArg;



static bbool bIdxServer_send(int fd, char* ePointer, size32_t size, int flags)
{
    
    char* lp_str =  (char*)ePointer;
    while(size)
    {

        int n_send = send(fd, lp_str, size, flags | MSG_NOSIGNAL);
        
        // printf("n_send : %d size :%d\n", n_send, size);
        if(n_send == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("bIdxServer_send");
            return EGG_FALSE;
        }
        else
        {
            size -= n_send;
            lp_str += n_send;
        }
    }

    return TRUE;
}

static bbool bIdxServer_recv(int fd, epointer ePointer, size32_t size, int flags)
{


    char* lp_str =  (char*)ePointer;
    while(size)
    {
        int n_recv = recv(fd, lp_str, size, flags);

        if(n_recv == -1)
        {
            if (errno == EINTR || errno == EAGAIN)
            {
                continue;
            }
            perror("bIdxServer -1 ");
            
            return EGG_FALSE;
        }
        else if(n_recv == 0)
        {
            perror("bIdxServer 0 ");
            return EGG_FALSE;
        }   
        else
        {
            size -= n_recv;
            lp_str += n_recv;
        }
    }

    
    return TRUE;
}



static int bIdxServer_set_non_blocking (int sfd)
{
  int flags, ret;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  ret = fcntl (sfd, F_SETFL, flags);
  if (ret == -1)
    {
      perror ("fcntl");
      return -1;
    }

  return 0;
}

static int bIdxServer_create_and_bind (char* ip, char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, sfd;

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     
  hints.ai_socktype = SOCK_STREAM; 
  hints.ai_flags = AI_PASSIVE;     

  s = getaddrinfo (ip, port, &hints, &result);
  if (s != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
      return -1;
    }

  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1)
        continue;
      
      int sock_reuse=1;
      if(setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR,(char *)&sock_reuse,sizeof(sock_reuse)) != 0)
          perror("setsockopt");

      
      s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
      if (s == 0)
        {
          break;
        }

      close (sfd);
    }

  if (rp == NULL)
    {
      fprintf (stderr, "Could not bind\n");
      return -1;
    }

  freeaddrinfo (result);

  return sfd;
}


bIdxServer* bIdxServer_init(char* ip, char *port)
{
    bIdxServer* lp_server = (bIdxServer*)malloc(sizeof(bIdxServer));
    lp_server->sfd = bIdxServer_create_and_bind (ip, port);
    lp_server->pool = bIdxThrPool_init(BIDX_MAX_WORKER);
    
    if (lp_server->sfd == -1)
    {
        printf("create_and_bind is false [%s][%d]!\n", __FILE__, __LINE__);
        abort ();
    }
      
    if( bIdxServer_set_non_blocking(lp_server->sfd) == -1)
    {
        printf("bIdxServer_set_non_blocking is false [%s][%d]!\n", __FILE__, __LINE__);
        abort ();   
    }
    

    if(listen (lp_server->sfd, BIDX_MAXEVENTS) == -1)
    {
      perror ("listen");
      abort ();
        
    }

    lp_server->efd = epoll_create1 (0);
    if (lp_server->efd == -1)
    {
        perror ("epoll_create");
        abort ();
    }

    struct epoll_event elisten;
    elisten.data.fd = lp_server->sfd;
    elisten.events = EPOLLIN | EPOLLET;
    if (epoll_ctl (lp_server->efd, EPOLL_CTL_ADD, lp_server->sfd, &elisten) == -1)
    {
        perror ("epoll_ctl");
        abort ();
    }

    lp_server->events = calloc (BIDX_MAXEVENTS, sizeof (struct epoll_event));
    
    return lp_server;
}

void* bIdxServer_processing_sk (void* arg)
{
    printf("  ==============================  bIdxServer_processing_sk  =======================%lu======= \n", pthread_self());
    bIdxServerArg* lp_arg = (bIdxServerArg*)arg;
    int fd = lp_arg->fd;
    bIdxer* pIdxer = lp_arg->idxer;
    int n_recv_sz = 0;
    bIdxServer_recv(fd, &n_recv_sz, sizeof(n_recv_sz), 0);
    n_recv_sz = ntohl(n_recv_sz);
    char* lp_recv_buf = (char* ) malloc(n_recv_sz + 1);
    bIdxServer_recv(fd, lp_recv_buf, n_recv_sz, 0);
    lp_recv_buf[n_recv_sz] = '\0';
    //   char* lp_res = bIdxer_query(pIdxer, "{\"op\":\"ADDTAG\", \"org\" : \"xian\", \"fieldId\" : \"LABEL\", \"tagId\" : [\"bf1\", \"bf2\",\"bf3\", \"bf4\", \"bf5\"] }");

    char* lp_res = bIdxer_query(pIdxer, lp_recv_buf);    
    int n_host_send_sz = strlen(lp_res) + 1;
    int n_net_send_sz = htonl(n_host_send_sz);
    char* lp_send_buf = (char* ) malloc(n_host_send_sz + sizeof(n_host_send_sz));
    
    memcpy(lp_send_buf, &n_net_send_sz, sizeof(n_net_send_sz));
    memcpy(lp_send_buf + sizeof(n_net_send_sz), lp_res, n_host_send_sz);
    
    bIdxServer_send(fd, lp_send_buf, n_host_send_sz + sizeof(n_host_send_sz), 0);
    free(lp_send_buf);
    free(lp_res);
    free(arg);
    close(fd);
    return 0;
}


int bIdxServer_request (bIdxServer* pServer, char* eqlIp, unsigned short eqlPort)
{

    int efd = pServer->efd;
    int sfd = pServer->sfd;
    struct epoll_event event;
    struct epoll_event *events = pServer->events;

    bIdxer* lp_idxer = bIdxer_new("./bidxer.test", eqlIp, eqlPort);

    printf("load OK !\n");
    
    while (1)
    {
        int n, i;

        n = epoll_wait(efd, events, BIDX_MAXEVENTS, -1);
        for (i = 0; i < n; i++)
        {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
              (!(events[i].events & EPOLLIN)))
            {
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            }
            
            else if (sfd == events[i].data.fd)
            {
                while (1)
                {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
                    
                    in_len = sizeof in_addr;
                    infd = accept (sfd, &in_addr, &in_len);
                    if (infd == -1)
                    {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))
                        {
                            break;
                        }
                        else
                        {
                            perror ("accept");
                            break;
                        }
                    }

                    int s = getnameinfo (&in_addr, in_len,
                                     hbuf, sizeof (hbuf),
                                     sbuf, sizeof (sbuf),
                                     NI_NUMERICHOST | NI_NUMERICSERV);
                    if (s == 0)
                    {
                        //                       printf("Accepted connection on descriptor %d "
//                             "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }

                    bIdxServerArg* lp_arg = (bIdxServerArg*)malloc(sizeof(bIdxServerArg));
                    lp_arg->fd = infd;
                    lp_arg->idxer = lp_idxer;
                    
                    bIdxThrPool_working(pServer->pool, lp_arg, bIdxServer_processing_sk);

                    
                }
              continue;
            }
          else
            {
                
//                printf ("Closed connection on descriptor %d\n", events[i].data.fd);

                //              close (events[i].data.fd);
            }
        }
    }

  free (events);

  close (sfd);

  return EXIT_SUCCESS;
}


/* int bIdxServer_request (bIdxServer* pServer) */
/* { */

/*     int efd = pServer->efd; */
/*     int sfd = pServer->sfd; */
/*     struct epoll_event event; */
/*     struct epoll_event *events = pServer->events; */

/*     bIdxer* lp_idxer = bIdxer_new("./bidxer.test"); */


    
/*     while (1) */
/*     { */
/*         int n, i; */

/*         n = epoll_wait(efd, events, BIDX_MAXEVENTS, -1); */
/*         for (i = 0; i < n; i++) */
/*         { */
/*             if ((events[i].events & EPOLLERR) || */
/*                 (events[i].events & EPOLLHUP) || */
/*               (!(events[i].events & EPOLLI ))) */
/*             { */
/*                 fprintf (stderr, "epoll error\n"); */
/*                 close (events[i].data.fd); */
/*                 continue; */
/*             } */
            
/*             else if (sfd == events[i].data.fd) */
/*             { */
/*                 while (1) */
/*                 { */
/*                     struct sockaddr in_addr; */
/*                     socklen_t in_len; */
/*                     int infd; */
/*                     char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV]; */
                    
/*                     in_len = sizeof in_addr; */
/*                     infd = accept (sfd, &in_addr, &in_len); */
/*                     if (infd == -1) */
/*                     { */
/*                         if ((errno == EAGAIN) || */
/*                             (errno == EWOULDBLOCK)) */
/*                         { */
/*                             break; */
/*                         } */
/*                         else */
/*                         { */
/*                             perror ("accept"); */
/*                             break; */
/*                         } */
/*                     } */

/*                     int s = getnameinfo (&in_addr, in_len, */
/*                                      hbuf, sizeof (hbuf), */
/*                                      sbuf, sizeof (sbuf), */
/*                                      NI_NUMERICHOST | NI_NUMERICSERV); */
/*                     if (s == 0) */
/*                     { */
/*                         printf("Accepted connection on descriptor %d " */
/*                              "(host=%s, port=%s)\n", infd, hbuf, sbuf); */
/*                     } */

/*                   s = bIdxServer_set_non_blocking (infd); */
/*                   if (s == -1) */
/*                     abort (); */

/*                   event.data.fd = infd; */
/*                   event.events = EPOLLIN | EPOLLET; */
/*                   s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event); */
/*                   if (s == -1) */
/*                     { */
/*                       perror ("epoll_ctl add"); */
/*                       abort (); */
/*                     } */
/*                 } */
/*               continue; */
/*             } */
/*           else */
/*             { */
/*                 int s = epoll_ctl (efd, EPOLL_CTL_DEL, events[i].data.fd, events + i); */
/*                 if (s == -1) */
/*                 { */
/*                     perror ("epoll_ctl del"); */
/*                     abort (); */
/*                 } */

/*                 bIdxServerArg* lp_arg = (bIdxServerArg*)malloc(sizeof(bIdxServerArg)); */
/*                 lp_arg->fd = events[i].data.fd; */
/*                 lp_arg->idxer = lp_idxer; */
                
/*                 bIdxThrPool_working(pServer->pool, lp_arg, bIdxServer_processing_sk); */
                
/* //                printf ("Closed connection on descriptor %d\n", events[i].data.fd); */

/*                 //              close (events[i].data.fd); */
/*             } */
/*         } */
/*     } */

/*   free (events); */

/*   close (sfd); */

/*   return EXIT_SUCCESS; */
/* } */

int main (int argc, char* argv[])
    
{
    if(argc != 3 && argc != 5)
    {
        printf("arg is error !\n");
        exit(-1);
    }
    char* ip = argv[1];
    char *port = argv[2];
    bIdxServer* lp_idx_server = bIdxServer_init(ip, port);
    if(argc == 3)
    {
        bIdxServer_request (lp_idx_server, "127.0.0.1", 30001);
    }
    else
    {
        bIdxServer_request (lp_idx_server, argv[3], atoi(argv[4]));
    }
    return 0;
}

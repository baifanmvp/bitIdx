#include <stdio.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include "../src/bIdxer.h"

char * sock_tran(char* js, char* ip, unsigned short port)
{
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in c_addr = {0};
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(port);
    
    if( !inet_aton(ip,&c_addr.sin_addr))
           perror("bad address");
    int ret = connect(sd, &c_addr, sizeof(c_addr) );

    if(ret == -1)
    {
        perror("connect");
    }

    int n_host_send = strlen(js) + 1;
    printf("n_host_send %d %d\n", strlen(js), n_host_send);
    int n_net_send = htonl(n_host_send);
    send(sd, &n_net_send, sizeof(n_net_send), 0);
    send(sd, js, n_host_send, 0);

    
    int n_recv = 0;
    recv(sd, &n_recv, sizeof(n_recv), 0);
    n_recv = ntohl(n_recv);
    char* lp_res = (char*)malloc(n_recv);
    recv(sd, lp_res, n_recv, 0);
    
    close(sd);
    return lp_res;
}

int main(int argc, char* argv[])
{

    if(argc != 3)
    {
        printf("arg is error !\n");
        exit(-1);
    }
    char* ip = argv[1];
    unsigned short port = (unsigned short)atoi(argv[2]);
    char* cmd;
    size_t sz = 0;
    while(1)
    {
        cmd = readline("cmd > ");
        
        
        if (!cmd)
            break;
        add_history(cmd);
 
        
        printf("%s\n", cmd);
        if(strcmp(cmd, "q") == 0)break;
        
        char* result = sock_tran(cmd, ip, port);
        
        printf("result : \n%s \n", result);
        free(result);
        free(cmd);
    }
    free(cmd);
    return 0;
}

#include <stdio.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../src/bIdxer.h"

int main()
{
    unlink("./bidxer.test");
    bIdxer* lp_idxer = bIdxer_new("./bidxer.test", "192.168.1.128", 30001);
    char* cmd;
    size_t sz = 0;
    while(1)
    {
        cmd = readline("cmd > ");
        
        
        if (!cmd)
            break;
        add_history(cmd);
 
        
        printf("%s\n", cmd);
        char* result = bIdxer_query(lp_idxer, cmd);
        printf("result : \n%s \n", result);
    }
    bIdxer_delete(lp_idxer);
    return 0;
}

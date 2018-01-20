#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "byte_range_locking.h"
enum class ExecProcess{
    RECORD_LOCK,MEMMAP
};

int main(int argc,char** argv)
{
    int arg1=atoi(argv[1]);
    switch(arg1)
    {
    case (int)ExecProcess::RECORD_LOCK:
    {
        pid_t pid= fork();
        if(pid<0)
            exit(-1);
        else if(pid>0)
        {
            printf("parent\n");
            wait_child(pid);
            father_process();
        }
        else {
            printf("child\n");
            pid_t pid2=fork();
            if(pid2!=0)
                exit(0);
            child_process_rclock();
            exit(0);
        }
        break;
    }
    case (int)ExecProcess::MEMMAP:
    {

        break;
    }
    default :
        break;
    }

    return 0;
}


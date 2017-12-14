#ifndef PIPE_H
#define PIPE_H
#include<unistd.h>
//pope只能用于父子进程之间通信，无父子进程之间的关系的进程无法使用pipe
class Pipe
{
public:
    Pipe();
    private:
    int pipe_fd[2];
};

#endif // PIPE_H

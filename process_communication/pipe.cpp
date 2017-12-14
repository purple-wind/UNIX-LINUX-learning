#include "pipe.h"
#include <iostream>
Pipe::Pipe()
{
 if(pipe(pipe_fd)<0)
 {
   return;
 }
 std::cout<<"fd[0]="<<pipe_fd[0]<<std::endl;
 std::cout<<"fd[1]="<<pipe_fd[1]<<std::endl;
 return;
}


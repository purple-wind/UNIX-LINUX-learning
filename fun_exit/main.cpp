/*linux-3.13.0 gcc-5.3.0*/

/* 程序执行到main函数结尾处，并不意味着程序已经结束；C程序启动时，内核调用exec启动特殊启动例程，
 *特殊启动例程将执行exit(main(argc,argv));C语言的写法，实际上该语句通常是由汇编写成的；
 *当exit被执行时，exit先执行一些清除处理(包括调用执行各种终止处理程序，关闭所有标准I/O流等)，
 *然后进入内核；atexit函数可以注册一些终止处理函数在exit执行时被调用；
 */
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
static void my_exit1(void)
{
 printf("first exit handler!\n");
}
static void my_exit2(void)
{
 printf("second exit handler!\n");
}

int main(void)
{
 if(atexit(my_exit2)!=0)
  printf("atexit my_exit2 fail!");
 if(atexit(my_exit1)!=0)
  printf("atexit my-exit1 fail!");
 printf("main is done!\n");
 return 0;
}

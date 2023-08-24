#include<pthread.h>
#include<signal.h>
#include<unistd.h>
#include<iostream>
//注意:编译该代码时需要链接pthread库，否则进程不会死锁
pthread_mutex_t lock;
void cb(int sig)
{
    if(sig==SIGALRM)
    {
        std::cout<<"----即使死锁也阻止不了定时器事件----"<<std::endl;
        pthread_mutex_unlock(&lock);
    }
}
int main()
{
	signal(SIGALRM,cb);
    alarm(2);
	//死锁是可中断睡眠，可被其它信号中断掉，比如定时器超时信号就可中断该状态
	//使用top工具查看死锁时进程是S状态而不是D状态
    pthread_mutex_lock(&lock);
    pthread_mutex_lock(&lock);
	std::cout<<"after deadlock"<<std::endl;

    return 0;
}

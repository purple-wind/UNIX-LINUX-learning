#include<pthread.h>
#include<signal.h>
#include<unistd.h>
#include<iostream>
#include<chrono>

/** 该进程启动2个子线程，一个主线程，当设置了SIGINT处理函数后，键盘输出ctrl+c，该信号会发送到每个线程，造成3个线程的sleep调用中断
 * 当主线程sleep(50)秒后向指定线程发送信号，只有指定线程可收到
 */
using namespace std;
std::chrono::steady_clock::duration debug_time(std::chrono::steady_clock::duration t1)
{
    //std::chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now().time_since_epoch() - chrono::steady_clock::now().time_since_epoch()).count();
    chrono::steady_clock::duration t2 = std::chrono::steady_clock::now().time_since_epoch();
    std::cout<<"used time="<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;
    return t2;
}
void signal_hadler1(int sig)
{
    std::cout<<__FUNCTION__<<" sig "<< sig<< std::endl;
}

void* work1(void* args)
{
    while(true)
    {
        sleep(200);
        int* t = (int*)args;
        auto t0 = std::chrono::steady_clock::now().time_since_epoch();
        for(int i = 0; i < *t; i++)
        {
        }
        auto t1 = debug_time(t0);
        break;
    }

    cout<<"work1 stop"<<endl;
}

void* work2(void* args)
{
    while(true)
    {
        sleep(100);
        int* t = (int*)args;
        auto t0 = std::chrono::steady_clock::now().time_since_epoch();
        for(int i = 0; i < *t; i++)
        {
        }
        auto t1 = debug_time(t0);
        break;
    }
    cout<<"work2 stop"<<endl;
}


int main(int argc, char* argv[])
{
    signal(SIGINT, signal_hadler1);
    int t = stoi(argv[1]);
    pthread_t pid1;
    pthread_create(&pid1, nullptr, work1, (void*)&t);
    pthread_detach(pid1);
    pthread_t pid2;
    pthread_create(&pid2, nullptr, work2, (void*)&t);
    pthread_detach(pid2);

    while(true)
    {
        sleep(50);
        cout<<"after sleep"<<endl;
        pthread_kill(pid1, SIGINT);
        pthread_kill(pid2, SIGINT);
    }
    return 0;
}

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * date:2018/1/23
 */
#include "shm.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
Shm::Shm(const char* arg_path, const int arg_id):path(arg_path),pro_id(arg_id)
{
    memset(&shmid_info,0x00,sizeof(shmid_info));

    //    build segment for data
    data_key=ftok(path.c_str(),pro_id);
    if(data_key<0)
    {
        perror("ftok");
        return;
    }
    int temp_shmid=shmget(data_key,0,0600);
    if(temp_shmid<0)
    {
        perror("shmget data");
        temp_shmid=shmget(data_key,4096,IPC_CREAT|IPC_EXCL|0600);//build new segment
        if(temp_shmid<0)
        {
            if(errno!=EEXIST)
            {
                printf("shmget data return\n");
                return;
            }
        }
    }
    shm_data_id=temp_shmid;



    //build segment for mutex_lock,be used process synchronize
    mutex_key=ftok(path.c_str(),pro_id+1);
    if(mutex_key<0)
    {
        perror("ftok");
        return;
    }
    temp_shmid=shmget(mutex_key,0,0600);
    if(temp_shmid<0)
    {
        perror("shmget mutex");
        temp_shmid=shmget(mutex_key,sizeof(pthread_mutex_t),IPC_CREAT|IPC_EXCL|0600);
        if(temp_shmid<0)
        {
            if(errno!=EEXIST)
            {
                perror("shmget mutex");
                printf("shmget mutex return\n");
                return;
            }
        }
    }
    shm_mutex_id=temp_shmid;
}

Shm::~Shm()
{
    unattach_shm();
}


bool Shm::attach_shm()
{
    if(shm_data_id==-1 || shm_mutex_id==-1)
        return false;
    data_addr=shmat(shm_data_id,nullptr,0);
    mutex_addr=(pthread_mutex_t*)shmat(shm_mutex_id,nullptr,0);
    process_share_mutex_init();
    return true;
}

bool Shm::unattach_shm()
{
    if(data_addr!=nullptr)
    {
        shmdt(data_addr);
    }
    if(mutex_addr!=nullptr)
    {
        shmdt(mutex_addr);
    }
    return true;
}

void Shm::delete_shm()
{
    shmctl(shm_data_id,IPC_RMID,nullptr);
    shmctl(shm_mutex_id,IPC_RMID,nullptr);
}

void Shm::get_shminfo()
{
    shmctl(shm_data_id,IPC_STAT,&shmid_info);
    printf("current nattch=%d\n",shmid_info.shm_nattch);
}

pthread_mutex_t Shm::process_share_mutex_init()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);//进程间共享属性,设置为进程间共享
    pthread_mutexattr_setrobust(&attr,PTHREAD_MUTEX_ROBUST);//设置锁的健壮性属性，设置为ROBUST
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);//设置锁的类型属性,设置为嵌套锁
    pthread_mutex_init(mutex_addr,&attr);
}

int Shm::process_lock()
{
    return pthread_mutex_lock(mutex_addr);
}

int Shm::process_unlock()
{
    return pthread_mutex_unlock(mutex_addr);
}


void process_test()
{
    Shm share_memory;
    share_memory.attach_shm();
    //多个进程同时操作共享内存，需要采取同步措施保证数据安全;
    for(int i=0;i<100000000;i++)
    {
        share_memory.process_lock();
        (*(uint64_t*)share_memory.data_addr)++;
        share_memory.process_unlock();
    }
    printf("final value=%lld\n",*(int*)share_memory.data_addr);
}

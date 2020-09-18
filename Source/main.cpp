//c include
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


//third party include
#include <wiringPi.h>

//self include
#include "tool.h"
#include "log.h"
#include "dht11.h"
#include "relay.h"
#include "thread.h"

//cpp include
#include <vector>

int read(int *t, int *m);

int main(int argc, char *argv[])
{
    //GPIO设置
    wiringPiSetup();  

    Threads::EpollThread epollThread;
    Threads::SqlThread sqlThread;
    epollThread.Start();
    sqlThread.Start();

    //主线程需要等待两个线程都结束之后再退出,不然会产生segment fault问题.
    epollThread.Join();
    sqlThread.Join();
    return 0;
}

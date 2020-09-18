#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <vector>
#include <iostream>

#include "thread.h"
#include "tool.h"
#include "log.h"
#include "relay.h"
#include "dht11.h"
#include "sensor.h"
#include "myysql.h"

#define MAXBUFFER 1024
#define LISTENQ 20

#define RELAY_PIN 0
#define DHT11_1_PIN 1
#define DHT11_2_PIN 2

namespace Threads
{

    /****************************************/
    /***************IThread******************/
    /****************************************/
    IThread::IThread()
        : _threadAttatched(false), _status(ThreadInitial)
    {
    }

    IThread::~IThread()
    {
        Stop();
    }

    void IThread::Start()
    {
        pthread_create(&_thread, NULL, _ThreadFunc, this);
        _threadAttatched = true;
    }

    void IThread::Join()
    {
        if (_threadAttatched)
        {
            pthread_join(_thread, NULL);
            _threadAttatched = false;
        }
    }

    void IThread::Cancel()
    {
        if (_threadAttatched)
        {
            pthread_cancel(_thread);
            _status = ThreadFinished;
        }
    }

    void IThread::Stop()
    {
        if (_threadAttatched)
        {
            pthread_detach(_thread); // 分离线程，子线程结束后自己回收资源
            _threadAttatched = false;
            _status = ThreadFinished;
        }
    }

    pthread_t IThread::GetThreadId() const
    {
        return _thread;
    }

    IThread::ThreadStatus IThread::GetThreadStatus() const
    {
        return _status;
    }

    void *IThread::_ThreadFunc(void *arg)
    {
        (reinterpret_cast<IThread *>(arg))->_Run();
        return NULL;
    }

    /****************************************/
    /***************EpollThread**************/
    /****************************************/
    EpollThread::EpollThread()
        : IThread()
    {
    }

    EpollThread::~EpollThread()
    {
        Stop();
    }

    void EpollThread::_Run()
    {

        int portnumber = atoi("7100");

        int listenfd, connfd, epfd, nfds;
        int sockfd;
        int n;
        char buf[MAXBUFFER];
        socklen_t clilen;
        struct sockaddr_in cliaddr, servaddr;

        struct epoll_event ev, events[20];

        epfd = epoll_create(24);

        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            errsys("can't create listenfd\n");
            return;
        }
        ev.data.fd = listenfd;
        ev.events = EPOLLIN;

        //将监听描述符添加进epfd
        epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

        //设置服务器端监听地址
        memset((void *)&servaddr, 0, sizeof(servaddr));
        //bzero((void*) &servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(portnumber);

        //将监听描述符和serveraddr绑定
        if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
        {
            printf("errno is %d\n", errno);
            errsys("can't bind listenfd to servaddr\n");
            return;
        }

        if (listen(listenfd, LISTENQ) == -1)
        {
            errsys("listen failed\n");
            return;
        };
        trace("now listening port:%d\n", portnumber);

        // 添加一个继电器类,监听然后控制
        Relay relay(RELAY_PIN);

        //进入epoll监听环节
        while (1)
        {
            nfds = epoll_wait(epfd, events, 20, 500);
            for (int i = 0; i < nfds; i++)
            {

                //监听的端口有新连接
                if (events[i].data.fd == listenfd)
                {
                    clilen = sizeof(cliaddr);
                    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                    if (connfd < 0)
                    {
                        errsys("connfd < 0\n");
                        close(listenfd);
                        return;
                    }

                    char *str = inet_ntoa(cliaddr.sin_addr);
                    int port = ntohs(cliaddr.sin_port);
                    trace("accept a connection from %s:%d \n", str, port);

                    ev.data.fd = connfd;
                    ev.events = EPOLLIN;

                    epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
                }
                else if (events[i].events & EPOLLIN)
                {
                    //有读事件,先将缓冲区置为/0;
                    memset(buf, 2, MAXBUFFER);
                    if ((sockfd = events[i].data.fd) < 0)
                    {
                        continue;
                    }
                    if ((n = read(sockfd, buf, MAXBUFFER)) < 0)
                    {
                        if (errno == ECONNRESET)
                        {
                            close(sockfd);
                            events[i].data.fd = -1;
                        }
                        else
                        {
                            errsys("readline errno\n");
                        }
                    }
                    else if (n == 0)
                    {
                        close(sockfd);
                        trace("connection fd:%d closed\n", sockfd);
                        events[i].data.fd = -1;
                    }
                    //这里设定服务端每次只发一个字节
                    //只有接收长度为一个字节的时候,才会
                    else if (n == 1)
                    {
                        //添加结束符
                        buf[n] = '\0';

                        //处理得到的输入数据,如果为0,表示关闭,如果为1表示开启
                        if (buf[0] == '0')
                        {
                            trace("RELAY CLOSE\n");
                            relay.off();
                        }
                        else if (buf[0] == '1')
                        {
                            trace("RELAY OPEN\n");
                            digitalWrite(RELAY_PIN, HIGH);
                        }

                        // 随便发送点什么回去给客户端,懒得改client代码了hhhhhhh
                        memset(buf, '\0', sizeof(buf));
                        strcpy(buf, "just send you a test\n");
                        trace("waiting to write fd:%d\n", sockfd);
                        write(sockfd, buf, strlen(buf));
                        trace("writing finished\n");
                    }
                }
                else if (events[i].events & EPOLLOUT)
                {
                    //do nothing
                }
            }
        }
        return;
    }

    /****************************************/
    /***************SqlThread****************/
    /****************************************/
    SqlThread::SqlThread()
        : IThread()
    {
    }

    SqlThread::~SqlThread()
    {
        Stop();
    }

    void SqlThread::_Run()
    {
        while (1)
        {
            // 休息个1秒再上传吧
            sleep(1);

            int tem1, tem2, hum1, hum2;
            Sensor::Dht11 dht11_1(DHT11_1_PIN);
            Sensor::Dht11 dht11_2(DHT11_2_PIN);
            dht11_1.read(&hum1, &tem1);
            dht11_2.read(&hum2, &tem2);
            char buffer[100];


            // 搞个平均值,因为read有可能会读出0,显然是非法的
            int tem_avg = (tem1 && tem2) ? (tem1 + tem2) / 2 : (tem1 ? tem1 : tem2);
            int hum_avg = (hum1 && hum2) ? (hum1 + hum2) / 2 : (hum1 ? hum1 : hum2);

            sprintf(buffer, "update env set tem ='%d' , hum ='%d%', wind='100',datetime=now() where id = 1;", tem_avg, hum_avg);
            Myysql mysql;

            // 好像把,ip账号密码给暴露了......应该没啥大问题吧..
            mysql.Connect("120.26.187.166", "root", "123456", "XTBridge");
            // mysql.Query("update env set tem ='50' , hum ='65%', wind='4',datetime=now() where id = 1;");
            if (!mysql.Query(buffer))
            {
                trace("mysql query failed\n");
            }
        }
    }
} // namespace Threads
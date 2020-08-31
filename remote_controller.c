#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "tool.h"
#include "log.h"

#include <wiringPi.h>

#define MAXBUFFER 1024
#define LISTENQ 20

#define RELAY 0

int main(int argc, char *argv[])
{
    int portnumber;
    if (argc == 2)
    {
        if ((portnumber = atoi(argv[1])) < 0)
        {
            errsys("usage:%s portnumber\n", argv[0]);
            return 1;
        }
    }
    else
    {
        errsys("usage:%s portnumber\n", argv[0]);
        return 1;
    }

    printf("portnumber is %d\n", portnumber);

    int listenfd, connfd, epfd, nfds;
    int sockfd;
    int n;
    char buf[MAXBUFFER];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    struct epoll_event ev, events[20];

    epfd = epoll_create(256);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        errsys("can't create listenfd\n");
        return 1;
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
        return 1;
    }

    if (listen(listenfd, LISTENQ) == -1)
    {
        errsys("listen failed\n");
        return 1;
    };
    trace("now listening port:%d\n", portnumber);

    //将BCM_GPIO 17即11口设置为输出
    wiringPiSetup();
    pinMode(RELAY, OUTPUT);
    digitalWrite(RELAY, LOW);
    //进入epoll监听环节
    for (;;)
    {
        nfds = epoll_wait(epfd, events, 20, 500);
        int i;
        for (i = 0; i < nfds; i++)
        {

            //监听的端口有新连接
            if (events[i].data.fd == listenfd)
            {
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                if (connfd < 0)
                {
                    errsys("connfd < 0\n");
                    return 1;
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
                //添加结束符
                buf[n] = '\0';

                //处理得到的输入数据,如果为0,表示关闭,如果为1表示开启
                if (buf[0] == '0')
                {
                    trace("RELAY CLOSE\n");
                    digitalWrite(RELAY, LOW);
                }
                else if (buf[0] == '1')
                {
                    trace("RELAY OPEN\n");
                    digitalWrite(RELAY, HIGH);
                }
                
                //直接关闭文件描述符
            }
            else if (events[i].events & EPOLLOUT)
            {
                //do nothing
            }
        }
    }

    return 0;
}

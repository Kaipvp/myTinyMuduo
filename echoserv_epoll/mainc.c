#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "wrap.h"

#define BUFFER_SIZE 10
#define MAX_ENVENT_NUM 1024

struct epoll_event events[MAX_ENVENT_NUM];

// 把sockfd修改为非阻塞的 (对sockfd的修改适合放在Socket类中)
int setnonblocking(int sockfd)
{
    int old_option = fcntl(sockfd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(sockfd, F_SETFL, new_option);
    return old_option;
}

// 把fd上的可读事件注册到epollfd对应的epoller上（epoll操作相关的都可以放在EPollPoller类中）
void addfd(int epollfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;         
    // 注册fd上的可读事件
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

int main()
{
    // 地址和端口相关的函数可以封装在InetAddress类中
    const char *ip = "127.0.0.1";
    int port = 8000;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr.sin_addr);
    addr.sin_port = htons(port);

    // listenfd与接收新连接密不可分，与listenfd相关的都可以封装在Acceptor类中
    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
    Listen(listenfd, 5);

    // 与epollfd相关的操作都可以封装在EPollPoller类中
    
    int epollfd = epoll_create(5);
    addfd(epollfd, listenfd);

    // 启动事件循环。何时启动、退出循环的事都可以封装在EventLoop类中
    while (1)  
    {
        int num = epoll_wait(epollfd ,events, MAX_ENVENT_NUM, -1);

        char buf[10];
        memset(buf, '\0', BUFFER_SIZE);
        for (int i = 0; i < num; ++i)
        {
            int sockfd = events[i].data.fd;
            // 如果是listenfd上有可读事件发生，说明有新连接到来
            //（在muduo中,listenfd相关的事件的处理是在主线程中完成的）
            if (sockfd == listenfd)
            {
                // client_addr用于保存客户的地址信息，与地址相关的都可以封装在InetAddress类中
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                // 接收连接，可以封装在Acceptor类中
                int connfd = Accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
                // 把新接收的连接connfd注册到epoller中
                addfd(epollfd, connfd);
            }
            // 不是listenfd那就是connfd上的事件
            //（在muduo中,connfd上相关的事件的处理是在子线程中完成的,这里为了简单就没有开启子线程）
            else if (events[i].events & EPOLLIN)   
            {
                // 接收数据。接收数据和发送数据都可以封装在TcpConection类中
                int ret = recv(sockfd, buf, BUFFER_SIZE, 0);
                if (ret <= 0)        // 如果接收数据出错或者对方关闭
                {
                    // 关闭连接。封装在Socket中
                    close(sockfd); 
                }
                // 把接收到的数据发送给对方,也封装在TcpConnection中
                send(sockfd, buf, ret, 0);  
            }
        }
    }
    return 0;
}
#include "wrap.h"

void perr_exit(const char *str)
{
    perr_exit(str);
    exit(1);
}

int Socket(int __domain, int __type, int __protocol)
{
    int n;
    if ((n = socket(__domain, __type, __protocol)) < 0)
    {
        perr_exit("socket error");
    }

    return n;
}

int Bind(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen)
{
    int n;
    if ((n = bind(sockfd, addr, addrlen)) < 0)
    {
        perr_exit("bind error");
    }
    return n;
}

int Listen(int sockfd, int backlog)
{
    int n;
    if ((n = listen(sockfd, backlog)) < 0)
    {
        perr_exit("listen error");
    }
    return n;
}
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int n;
again:
    if ((n = accept(sockfd, addr, addrlen)) < 0)
    {
        if ((errno == ECONNABORTED) || (errno == EINTR))
        {
            goto again;
        }
        else
        {
            perr_exit("accept error");
        }
    }
    return n;
}
int Connect(int sockfd, const struct sockaddr *addr,
            socklen_t addrlen)
{
    int n;
    if ((n = connect(sockfd, addr, addrlen)) < 0)
    {
        perr_exit("connect error");
    }
    return n;
}
ssize_t Read(int fd, void *buf, size_t count)
{
    ssize_t n;
again:
    if ((n = read(fd, buf, count)) == -1)
    {
        if (errno == EINTR)
        {
            goto again;
        }
        else
        {
            return -1;
        }
    }
    return n;
}
ssize_t Write(int fd, const void *buf, size_t count)
{
    ssize_t n;
again:
    if ((n = write(fd, buf, count)) == -1)
    {
        if (errno == EINTR)
        {
            goto again;
        }
        else
        {
            return -1;
        }
    }
    return n;
}
int Close(int fd)
{
    int n;
    if ((n = close(fd)) == -1)
    {
        perror("close error");
    }

    return n;
}

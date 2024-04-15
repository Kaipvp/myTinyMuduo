#ifndef _WRAP_H_
#define _WRAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void perr_exit(const char *str);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr,
            socklen_t addrlen);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);

int Close(int fd);
#endif


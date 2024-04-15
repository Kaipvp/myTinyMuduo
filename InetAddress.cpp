#include "InetAddress.h"

InetAddress::InetAddress(uint16_t port, std::string ip)
{
    ::memset(&Addr_, 0, sizeof(Addr_));
    Addr_.sin_family = AF_INET;
    Addr_.sin_port = htons(port);
    Addr_.sin_addr.s_addr = ::inet_addr(ip.c_str());
}

std::string InetAddress::toIp() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &Addr_.sin_addr, buf, sizeof(buf));
    return buf;
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &Addr_.sin_addr, buf, sizeof(buf));
    size_t len = ::strlen(buf);
    uint16_t port = ::ntohs(Addr_.sin_port);
    printf("ip = ");
    sprintf(buf+len, ": port = %u", port);
    return buf;
}

uint16_t InetAddress::toPort() const
{
    return ntohs(Addr_.sin_port);
}

// #include <iostream>
// int main()
// {
//     InetAddress addr(8080);
//     std::cout << addr.toIpPort() << std::endl;
// }

//test: ip = 127.0.0.1: port = 8080
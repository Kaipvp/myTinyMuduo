#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

class InetAddress{
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &Addr)
        : Addr_(Addr)
        {}

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getSockAddr() const { return &Addr_; }
    void setSockAddr(const sockaddr_in& addr) { Addr_ = addr; }
    
private:
    sockaddr_in Addr_;
};
//
//  SocketAddress.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 01.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <netdb.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include "SocketAddress.hpp"

namespace Network {
    
    SocketAddress::SocketAddress() : SocketAddress(INADDR_ANY, 0) {}
    
    SocketAddress::SocketAddress(uint32_t host, uint16_t port) {
        sockaddr_in *sockAddr = AsSockAddrIn();
        memset(sockAddr, 0, Size());
        sockAddr->sin_family = AF_INET;
        sockAddr->sin_addr.s_addr = htonl(host);
        sockAddr->sin_port = htons(port);
    }
    
    SocketAddress::SocketAddress(const sockaddr &addr) {
        std::memcpy(&this->addr, &addr, Size());
        sockaddr_in *sockAddr = AsSockAddrIn();
        sockAddr->sin_port = htons(sockAddr->sin_port);
    }
    
    SocketAddress::SocketAddress(const SocketAddress &other) : SocketAddress(other.addr) {}
    
    std::shared_ptr<SocketAddress> SocketAddress::CreateIPv4(const std::string &hostName) {
        size_t pos = hostName.find_last_of(':');
        std::string host;
        std::string service;
        
        if (pos == std::string::npos) {
            host = hostName;
            service = "0";
        } else {
            host = hostName.substr(0, pos);
            service = hostName.substr(pos + 1);
        }
        addrinfo info;
        std::memset(&info, 0, sizeof(addrinfo));
        info.ai_family = AF_INET;
        
        addrinfo *result;
        int error = getaddrinfo(host.c_str(), service.c_str(), &info, &result);
        if (error != 0) {
            if (result != nullptr) {
                freeaddrinfo(result);
            }
            throw std::invalid_argument("SocketAddress::CreateIPv4 failed!");
        }
        while (result->ai_addr == nullptr && result->ai_next != nullptr) {
            result = result->ai_next;
        }
        if (result->ai_addr == nullptr) {
            freeaddrinfo(result);
            throw std::invalid_argument("SocketAddress::CreateIPv4 failed!");
        }
        std::shared_ptr<SocketAddress> address = std::make_shared<SocketAddress>(*result->ai_addr);
        freeaddrinfo(result);
        return address;
    }
    
    std::ostream &operator << (std::ostream &stream, const SocketAddress &address) {
        const sockaddr_in *addrIn = reinterpret_cast<const sockaddr_in *>(&address.addr);
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addrIn->sin_addr.s_addr, addr, INET_ADDRSTRLEN);
        stream << addr << ':' << addrIn->sin_port;
        return stream;
    }
    
}

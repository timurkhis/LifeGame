//
//  SocketAddress.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 01.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#if !defined(_WIN32)
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <string>
#include <iostream>
#include "SocketAddress.hpp"
#include "Utils.hpp"

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
    
    uint32_t SocketAddress::GetHost() {
        return static_cast<uint32_t>(AsSockAddrIn()->sin_addr.s_addr);
    }
    
    uint16_t SocketAddress::GetPort() {
        return static_cast<uint16_t>(AsSockAddrIn()->sin_port);
    }
    
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
        
        addrinfo *result = nullptr;
        int error = getaddrinfo(host.c_str(), service.c_str(), &info, &result);
        if (error != 0) {
            if (result != nullptr) {
                freeaddrinfo(result);
            }
            Log::Error("SocketAddress::CreateIPv4 failed!");
        }
        while (result->ai_addr == nullptr && result->ai_next != nullptr) {
            result = result->ai_next;
        }
        if (result->ai_addr == nullptr) {
            freeaddrinfo(result);
            Log::Error("SocketAddress::CreateIPv4 failed!");
        }
        std::shared_ptr<SocketAddress> address = std::make_shared<SocketAddress>(*result->ai_addr);
        freeaddrinfo(result);
        return address;
    }
    
    std::string SocketAddress::ToString() {
        std::stringstream stream;
        stream << *this;
        return stream.str();
    }
    
    std::ostream &operator << (std::ostream &stream, SocketAddress &address) {
        sockaddr_in *addrIn = reinterpret_cast<sockaddr_in *>(&address.addr);
        char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addrIn->sin_addr.s_addr, addr, INET_ADDRSTRLEN);
        stream << addr << ':' << addrIn->sin_port;
        return stream;
    }
    
}

//
//  SocketAddress.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 01.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef SocketAddress_hpp
#define SocketAddress_hpp

#if defined(_WIN32)
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
struct sockaddr_in;
#endif

#include <memory>
#include <string>
#include <stdint.h>

namespace Network {
    
    class SocketAddress {
        sockaddr addr;
        
    public:
        static std::shared_ptr<SocketAddress> CreateIPv4(const std::string &hostName);
        
        explicit SocketAddress();
        explicit SocketAddress(uint32_t host, uint16_t port);
        explicit SocketAddress(const sockaddr &addr);
        SocketAddress(const SocketAddress &other);
        
        socklen_t Size() const { return sizeof(addr); }
        
        uint32_t GetHost();
        uint16_t GetPort();
        
        sockaddr *AsSockAddr() { return &addr; }
        sockaddr_in *AsSockAddrIn() { return reinterpret_cast<sockaddr_in *>(&addr); }
        
        std::string ToString();
        friend std::ostream &operator << (std::ostream &stream, SocketAddress &address);
    };
    
}

#endif /* SocketAddress_hpp */

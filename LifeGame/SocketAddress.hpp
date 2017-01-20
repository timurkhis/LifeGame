//
//  SocketAddress.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 01.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef SocketAddress_hpp
#define SocketAddress_hpp

#include <sys/socket.h>
#include <stdint.h>
#include <memory>

struct sockaddr_in;

namespace Network {
    
    class SocketAddress {
        sockaddr addr;
        
    public:
        static std::shared_ptr<SocketAddress> CreateIPv4(const std::string &hostName);
        
        explicit SocketAddress();
        explicit SocketAddress(uint32_t host, uint16_t port);
        explicit SocketAddress(const sockaddr &addr);
        SocketAddress(const SocketAddress &other);
        
        constexpr socklen_t Size() const { return sizeof(addr); }
        
        sockaddr *AsSockAddr() { return &addr; }
        sockaddr_in *AsSockAddrIn() { return reinterpret_cast<sockaddr_in *>(&addr); }
        
        friend std::ostream &operator << (std::ostream &stream, const SocketAddress &address);
    };

}
#endif /* SocketAddress_hpp */

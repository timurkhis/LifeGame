//
//  TCPSocket.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 02.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "TCPSocket.hpp"

namespace Network {
    
    std::shared_ptr<TCPSocket> TCPSocket::Create() {
        const int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0) {
            throw std::exception_ptr();
        }
        return std::shared_ptr<TCPSocket>(new TCPSocket(sock));
    }
    
    TCPSocket::~TCPSocket() {
        close(sock);
    }
    
    int TCPSocket::Connect(SocketAddress &address) {
        return connect(sock, address.AsSockAddr(), address.Size());
    }
    
    int TCPSocket::Bind(SocketAddress &address) {
        return bind(sock, address.AsSockAddr(), address.Size());
    }
    
    int TCPSocket::Listen(int backLog) {
        return listen(sock, backLog);
    }
    
    int TCPSocket::Send(void *buffer, size_t len) {
        return send(sock, buffer, len, 0);
    }
    
    int TCPSocket::Recv(void *buffer, size_t len) {
        return recv(sock, buffer, len, 0);
    }
    
    std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress &address) {
        socklen_t addrlen;
        const int newSocket = accept(sock, address.AsSockAddr(), &addrlen);
        if (newSocket < 0) {
            throw std::exception_ptr();
        }
        return std::shared_ptr<TCPSocket>(new TCPSocket(newSocket));
    }
    
}

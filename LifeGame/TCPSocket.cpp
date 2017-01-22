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
#include "Log.hpp"
#include "TCPSocket.hpp"

namespace Network {
    
    TCPSocketPtr TCPSocket::Create() {
        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0) {
            Log::Error("TCPSocket::Create failed!");
        }
        return TCPSocketPtr(new TCPSocket(sock));
    }
    
    TCPSocket::~TCPSocket() {
        close(sock);
    }
    
    void TCPSocket::Addr(SocketAddress &address) {
        socklen_t addrlen = address.Size();
        int result = getsockname(sock, address.AsSockAddr(), &addrlen);
        if (result < 0) {
            Log::Error("TCPSocket::Addr failed!");
        }
    }
    
    void TCPSocket::Connect(SocketAddress &address) {
        int result = connect(sock, address.AsSockAddr(), address.Size());
        if (result < 0) {
            Log::Error("TCPSocket::Connect failed!");
        }
    }
    
    void TCPSocket::Bind(SocketAddress &address) {
        int result = bind(sock, address.AsSockAddr(), address.Size());
        if (result < 0) {
            Log::Error("TCPSocket::Bind failed!");
        }
    }
    
    void TCPSocket::Listen(int backLog) {
        int result = listen(sock, backLog);
        if (result < 0) {
            Log::Error("TCPSocket::Listen failed!");
        }
    }
    
    int TCPSocket::Send(void *buffer, size_t len) {
        int result = static_cast<int>(send(sock, buffer, len, 0));
        if (result < 0) {
            Log::Error("TCPSocket::Send failed!");
        }
        return result;
    }
    
    int TCPSocket::Recv(void *buffer, size_t len) {
        int result = static_cast<int>(recv(sock, buffer, len, 0));
        if (result < 0) {
            Log::Error("TCPSocket::Recv failed!");
        }
        return result;
    }
    
    TCPSocketPtr TCPSocket::Accept(SocketAddress address) {
        socklen_t addrlen = address.Size();
        int newSocket = accept(sock, address.AsSockAddr(), &addrlen);
        if (newSocket < 0) {
            Log::Error("TCPSocket::Accept failed!");
        }
        return TCPSocketPtr(new TCPSocket(newSocket));
    }
    
}

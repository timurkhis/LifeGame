//
//  TCPSocket.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 02.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
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
    
    void TCPSocket::NagleAlgorithm(bool enable) {
        int flag = enable ? 1 : 0;
        int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
        if (result < 0) {
            Log::Error("TCPSocket::NagleAlgorithm failed!");
        }
    }
    
    void TCPSocket::Addr(SocketAddress &address, bool remote) {
        socklen_t addrlen = address.Size();
        int result;
        if (remote) {
            result = getpeername(sock, address.AsSockAddr(), &addrlen);
        } else {
            result = getsockname(sock, address.AsSockAddr(), &addrlen);
        }
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
    
    int TCPSocket::Recv(void *buffer, size_t len, bool peek) {
        int result = static_cast<int>(recv(sock, buffer, len, peek ? MSG_PEEK : 0));
        if (result < 0) {//TODO: add error handler
            if (errno == ECONNRESET) {
                Log::Warning("TCPSocket::Recv connection reset!");
                result = 0;
            } else {
                Log::Error("TCPSocket::Recv failed!");
            }
        }
        return result;
    }
    
    int TCPSocket::DataSize() const {
        int size;
        int result = ioctl(sock, FIONREAD, &size);
        if (result < 0) {
            Log::Error("TCPSocket::DataSize failed!");
        }
        return size;
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

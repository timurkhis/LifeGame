//
//  TCPSocket.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 02.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef TCPSocket_hpp
#define TCPSocket_hpp

#if !defined(_WIN32)
typedef int SOCKET;
#endif

#include "SocketAddress.hpp"

namespace Network {
    
    class TCPSocket {
        friend class SocketSelector;
		SOCKET sock;

    public:
        static std::shared_ptr<TCPSocket> Create();
        
        ~TCPSocket();
        void NagleAlgorithm(bool enable);
        void Addr(SocketAddress &address, bool remote = false);
        void Connect(SocketAddress &address);
        void Bind(SocketAddress &address);
        void Listen(int backLog = SOMAXCONN);
        int Send(void *buffer, size_t len);
        int Recv(void *buffer, size_t len, bool peek = false);
        int DataSize() const;
        std::shared_ptr<TCPSocket> Accept(SocketAddress address);
        
    private:
        explicit TCPSocket(SOCKET socket) : sock(socket) {}
    };

    typedef std::shared_ptr<TCPSocket> TCPSocketPtr;
    
}
#endif /* TCPSocket_hpp */

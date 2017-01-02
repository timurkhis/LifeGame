//
//  TCPSocket.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 02.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef TCPSocket_hpp
#define TCPSocket_hpp

#include "SocketAddress.hpp"

namespace Network {
    
    class TCPSocket {
        int sock;
        
    public:
        static std::shared_ptr<TCPSocket> Create();
        
        ~TCPSocket();
        int Connect(SocketAddress &address);
        int Bind(SocketAddress &address);
        int Listen(int backLog = 32);
        int Send(void *buffer, size_t len);
        int Recv(void *buffer, size_t len);
        std::shared_ptr<TCPSocket> Accept(SocketAddress &address);
        
    private:
        explicit TCPSocket(int socket) : sock(socket) {}
    };

}
#endif /* TCPSocket_hpp */

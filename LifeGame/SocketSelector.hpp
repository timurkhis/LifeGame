//
//  SocketSelector.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 15.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef SocketSelector_hpp
#define SocketSelector_hpp

#include <vector>
#include "TCPSocket.hpp"

namespace Network {
    
    class SocketSelector {
    public:
        static int Select(const std::vector<TCPSocketPtr> *inRead,
                                std::vector<TCPSocketPtr> *outRead,
                          const std::vector<TCPSocketPtr> *inWrite,
                                std::vector<TCPSocketPtr> *outWrite,
                          const std::vector<TCPSocketPtr> *inExcept,
                                std::vector<TCPSocketPtr> *outExcept,
                          bool block = true);
        
        static int SelectRead(const std::vector<TCPSocketPtr> *inRead, std::vector<TCPSocketPtr> *outRead, bool block = true);
        static int SelectWrite(const std::vector<TCPSocketPtr> *inWrite, std::vector<TCPSocketPtr> *outWrite, bool block = true);
        static int SelectExcept(const std::vector<TCPSocketPtr> *inExcept, std::vector<TCPSocketPtr> *outExcept, bool block = true);
        
    private:
        static timeval *SetBlock(timeval *time, bool block);
        static fd_set *FillSet(const std::vector<TCPSocketPtr> *sockets, fd_set *set, int *maxSocket);
        static void FillVector(const std::vector<TCPSocketPtr> *sockets, std::vector<TCPSocketPtr> *result, const fd_set *set);
    };
    
}

#endif /* SocketSelector_hpp */

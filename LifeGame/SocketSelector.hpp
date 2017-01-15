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
        std::vector<TCPSocketPtr> sockets;
        int maxSocket;
        
    public:
        void Add(TCPSocketPtr socketPtr);
        void Remove(TCPSocketPtr socketPtr);
        
        size_t Size() { return sockets.size(); }
        int Select(std::vector<TCPSocketPtr> *read, std::vector<TCPSocketPtr> *write, std::vector<TCPSocketPtr> *except);
        
    private:
        void FillSet(fd_set *set);
        void FillVector(std::vector<TCPSocketPtr> *result, const fd_set *set);
    };
    
}

#endif /* SocketSelector_hpp */

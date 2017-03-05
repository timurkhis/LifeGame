//
//  Connection.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Connection_hpp
#define Connection_hpp

#include "Network.h"

namespace Messaging {

    class Connection {
        bool canRead;
        bool canWrite;
        uint32_t recvMessages;
        uint32_t recvData;
        uint32_t allRecvData;
        uint32_t sendData;
        
    public:
        Network::TCPSocketPtr socket;
        Network::InputMemoryStream input;
        Network::OutputMemoryStream output;
        
        explicit Connection(Network::TCPSocketPtr socket);
        
        uint32_t RecvMessages() const { return recvMessages; }
        bool CanRead() const { return canRead; }
        bool CanWrite() const { return canWrite; }
        void Clear() { recvMessages = 0; }
        
        int Recv();
        int Send();
    };

    typedef std::shared_ptr<Connection> ConnectionPtr;
    
}

#endif /* Connection_hpp */

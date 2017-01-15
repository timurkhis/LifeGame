//
//  Network.h
//  LifeGame
//
//  Created by Максим Бакиров on 02.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Network_h
#define Network_h

#include "SocketAddress.hpp"
#include "TCPSocket.hpp"
#include "Reporter.hpp"
#include "MemoryStream.hpp"
#include "SocketSelector.hpp"

namespace Network {
    
    enum class Message {
        Init,
        Add,
        Turn,
        Process
    };
    
}

#endif /* Network_h */

//
//  SocketSelector.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 15.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <sys/select.h>
#include "Reporter.hpp"
#include "SocketSelector.hpp"

namespace Network {
    
    void SocketSelector::Add(TCPSocketPtr socketPtr) {
        sockets.push_back(socketPtr);
        maxSocket = std::max(maxSocket, socketPtr->sock);
    }
    
    void SocketSelector::Remove(TCPSocketPtr socketPtr) {
        auto sockToRemove = std::find(sockets.begin(), sockets.end(), socketPtr);
        if (sockToRemove != sockets.end()) {
            sockets.erase(sockToRemove);
        }
    }
    
    int SocketSelector::Select(std::vector<TCPSocketPtr> *read, std::vector<TCPSocketPtr> *write, std::vector<TCPSocketPtr> *except) {
        fd_set readSet;
        fd_set writeSet;
        fd_set exceptSet;
        FillSet(&readSet);
        FillSet(&writeSet);
        FillSet(&exceptSet);
        int result = select(maxSocket + 1, &readSet, &writeSet, &exceptSet, nullptr);
        if (result < 0) {
            Reporter::Report("SocketSelector::Select failed!");
        }
        FillVector(read, &readSet);
        FillVector(write, &writeSet);
        FillVector(except, &exceptSet);
        return result;
    }
    
    void SocketSelector::FillSet(fd_set *set) {
        FD_ZERO(set);
        for (int i = 0; i < sockets.size(); i++) {
            FD_SET(sockets[i]->sock, set);
        }
    }
    
    void SocketSelector::FillVector(std::vector<TCPSocketPtr> *result, const fd_set *set) {
        if (result == nullptr) return;
        result->clear();
        for (int i = 0; i < sockets.size(); i++) {
            if (FD_ISSET(sockets[i]->sock, set)) {
                result->push_back(sockets[i]);
            }
        }
    }
    
}

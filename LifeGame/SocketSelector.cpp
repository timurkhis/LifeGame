//
//  SocketSelector.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 15.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#if !defined(_WIN32)
#include <sys/select.h>
#endif

#include "SocketSelector.hpp"
#include "Utils.hpp"

namespace Network {
    
    int SocketSelector::Select(const std::vector<TCPSocketPtr> *inRead,
                                     std::vector<TCPSocketPtr> *outRead,
                               const std::vector<TCPSocketPtr> *inWrite,
                                     std::vector<TCPSocketPtr> *outWrite,
                               const std::vector<TCPSocketPtr> *inExcept,
                                     std::vector<TCPSocketPtr> *outExcept,
                               bool block) {
        int maxSocket = 0;
        timeval time;
        timeval *timePtr = SetBlock(&time, block);
        fd_set readSet, writeSet, exceptSet;
        fd_set *readPtr = FillSet(inRead, &readSet, &maxSocket);
        fd_set *writePtr = FillSet(inWrite, &writeSet, &maxSocket);
        fd_set *exceptPtr = FillSet(inExcept, &exceptSet, &maxSocket);
        int result = select(maxSocket + 1, readPtr, writePtr, exceptPtr, timePtr);
        if (result < 0) {
            Log::Error("SocketSelector::Select failed!");
        }
        FillVector(inRead, outRead, &readSet);
        FillVector(inWrite, outWrite, &writeSet);
        FillVector(inExcept, outExcept, &exceptSet);
        return result;
    }
    
    int SocketSelector::SelectRead(const std::vector<TCPSocketPtr> *inRead, std::vector<TCPSocketPtr> *outRead, bool block) {
        return Select(inRead, outRead, nullptr, nullptr, nullptr, nullptr, block);
    }
    
    int SocketSelector::SelectWrite(const std::vector<TCPSocketPtr> *inWrite, std::vector<TCPSocketPtr> *outWrite, bool block) {
        return Select(nullptr, nullptr, inWrite, outWrite, nullptr, nullptr, block);
    }
    
    int SocketSelector::SelectExcept(const std::vector<TCPSocketPtr> *inExcept, std::vector<TCPSocketPtr> *outExcept, bool block) {
        return Select(nullptr, nullptr, nullptr, nullptr, inExcept, outExcept, block);
    }
    
    timeval *SocketSelector::SetBlock(timeval *time, bool block) {
        if (block) return nullptr;
        std::memset(time, 0, sizeof(timeval));
        return time;
    }
    
    fd_set *SocketSelector::FillSet(const std::vector<TCPSocketPtr> *sockets, fd_set *set, int *maxSocket) {
        if (sockets == nullptr || sockets->size() == 0) return nullptr;
        FD_ZERO(set);
        for (int i = 0; i < sockets->size(); i++) {
            TCPSocketPtr socket = sockets->at(i);
#if !defined(_WIN32)
            *maxSocket = std::max(socket->sock, *maxSocket);
#endif
            FD_SET(socket->sock, set);
        }
        return set;
    }
    
    void SocketSelector::FillVector(const std::vector<TCPSocketPtr> *sockets, std::vector<TCPSocketPtr> *result, const fd_set *set) {
        if (sockets == nullptr || sockets->size() == 0 || result == nullptr) return;
        result->clear();
        for (int i = 0; i < sockets->size(); i++) {
            TCPSocketPtr socket = sockets->at(i);
            if (FD_ISSET(socket->sock, set)) {
                result->push_back(socket);
            }
        }
    }
    
}

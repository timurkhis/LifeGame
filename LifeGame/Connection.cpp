//
//  Connection.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.02.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Connection.hpp"

using namespace Network;
    
Connection::Connection(TCPSocketPtr socket) :
    canRead(true),
    canWrite(true),
    input(1024),
    output(1024),
    recvData(0),
    allRecvData(0),
    sendData(0),
    socket(socket) {}

int Connection::Recv() {
    uint32_t dataSize = socket->DataSize();
    if (input.Capacity() - recvData < dataSize) {
        input.Realloc(std::max(input.Capacity() * 2, dataSize));
    }
    int result = socket->Recv(input.Data(recvData), input.Capacity() - recvData);
    recvData += result;
    if (allRecvData == 0) {
        input.Read(allRecvData, 0);
    }
    canRead = recvData == allRecvData;
    if (canRead) {
        recvData = 0;
        allRecvData = 0;
    }
    return result;
}

int Connection::Send() {
    int result = socket->Send(output.Data(sendData), output.Size() - sendData);
    sendData += result;
    canWrite = sendData == output.Size();
    if (canWrite) {
        sendData = 0;
    }
    return result;
}

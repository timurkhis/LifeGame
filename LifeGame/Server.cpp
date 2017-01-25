//
//  Server.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <thread>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include "Server.hpp"

using namespace Geometry;
using namespace Network;

Server::Server(Vector fieldSize, uint32_t capacity) :
    fieldSize(fieldSize),
    input(capacity),
    output(capacity) {
    address = SocketAddress::CreateIPv4("127.0.0.1");
    listener = TCPSocket::Create();
    listener->Bind(*address);
    listener->Listen();
    listener->Addr(*address);
    players.Add(listener);
    std::thread update(&Server::Update, this);
    update.detach();
}

void Server::Update() {
    std::vector<TCPSocketPtr> read;
    while (players.Select(&read, nullptr, nullptr) > 0) {
        auto newPlayer = std::find(read.begin(), read.end(), listener);
        if (newPlayer != read.end()) {
            AddPlayer();
            read.erase(newPlayer);
        }
        for (int i = 0; i < read.size(); i++) {
            int result = read[i]->Recv(input.Data(), input.Capacity());
            int player;
            std::vector<Vector> vec;
            Read<Message::Turn>(input, player, vec);
            if (result == 0) {
                RemovePlayer(player);
                players.Remove(read[i]);
            } else {
                addedUnits[player] = std::move(vec);
                playerTurns[player] = true;
            }
            input.Clear();
        }
        if (std::count_if(playerTurns.begin(), playerTurns.end(), [](bool value){ return value; }) == playerTurns.size()) {
            Write<Message::Process>(output, addedUnits);
            for (int i = 0; i < players.Size(); i++) {
                TCPSocketPtr ptr = players.At(i);
                if (ptr != listener) {
                    ptr->Send(output.Data(), output.Size());
                }
            }
            for (int i = 0; i < playerTurns.size(); i++) {
                playerTurns[i] = false;
            }
            output.Clear();
        }
    }
}

void Server::AddPlayer() {
    TCPSocketPtr newPlayer = listener.get()->Accept(*address);
    int result = static_cast<int>(players.Size() - 1);
    Write<Message::Init>(output, result, fieldSize);
    newPlayer->Send(output.Data(), output.Size());
    output.Clear();
    addedUnits.emplace_back();
    playerTurns.push_back(false);
    players.Add(newPlayer);
}

void Server::RemovePlayer(int player) {
    playerTurns.erase(playerTurns.begin() + player);
    addedUnits.erase(addedUnits.begin() + player);
}

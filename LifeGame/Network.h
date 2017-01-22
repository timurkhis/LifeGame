//
//  Network.h
//  LifeGame
//
//  Created by Максим Бакиров on 02.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Network_h
#define Network_h

#include "Vector.hpp"
#include "SocketAddress.hpp"
#include "TCPSocket.hpp"
#include "Log.hpp"
#include "MemoryStream.hpp"
#include "SocketSelector.hpp"

namespace Network {
    
    enum class Message {
        Init,
        Turn,
        Process
    };
    
    template <Message Type, typename ...Args>
    struct Msg {};
    
    template <Message Type, typename ...Args>
    void Read(InputMemoryStream &stream, Args &... args) {
        Msg<Type, Args...>().Read(stream, args...);
    }
    
    template <Message Type, typename ...Args>
    void Write(OutputMemoryStream &stream, Args &... args) {
        Msg<Type, Args...>().Write(stream, args...);
    }
    
    template <>
    struct Msg<Message::Init, int, Geometry::Vector> {
        void Read(InputMemoryStream &stream, int &player, Geometry::Vector &fieldSize) {
            int32_t type, number, x, y;
            stream >> type >> number >> x >> y;
            if (static_cast<Message>(type) != Message::Init) {
                Log::Error("Messages types are not the same!", type);
            }
            player = static_cast<int>(number);
            fieldSize = Geometry::Vector(static_cast<int>(x), static_cast<int>(y));
        }
        
        void Write(OutputMemoryStream &stream, int &player, Geometry::Vector &fieldSize) {
            stream << static_cast<int32_t>(Message::Init) << static_cast<int32_t>(player);
            stream << static_cast<int32_t>(fieldSize.x) << static_cast<int32_t>(fieldSize.y);
        }
    };
    
    template <>
    struct Msg<Message::Turn, int, std::vector<Geometry::Vector>> {
        void Read(InputMemoryStream &stream, int &player, std::vector<Geometry::Vector> &units) {
            int32_t type, number, size;
            stream >> type >> number >> size;
            if (static_cast<Message>(type) != Message::Turn) {
                Log::Error("Messages types are not the same!", type);
            }
            player = static_cast<int>(number);
            units.reserve(size);
            for (int i = 0; i < size; i++) {
                int32_t x, y;
                stream >> x >> y;
                units.push_back(Geometry::Vector(static_cast<int>(x), static_cast<int>(y)));
            }
        }
        
        void Write(OutputMemoryStream &stream, int &player, std::vector<Geometry::Vector> &units) {
            stream << static_cast<int32_t>(Message::Turn) << static_cast<int32_t>(player) << static_cast<int32_t>(units.size());
            for (int i = 0; i < units.size(); i++) {
                Geometry::Vector vec = units[i];
                stream << static_cast<int32_t>(vec.x) << static_cast<int32_t>(vec.y);
            }
        }
    };
    
    template <>
    struct Msg<Message::Process, std::vector<std::vector<Geometry::Vector>>> {
        void Read(InputMemoryStream &stream, std::vector<std::vector<Geometry::Vector>> &newUnits) {
            int32_t type, units;
            stream >> type >> units;
            if (static_cast<Message>(type) != Message::Process) {
                Log::Error("Messages types are not the same!", type);
            }
            for (int i = 0; i < units; i++) {
                int32_t size;
                stream >> size;
                std::vector<Geometry::Vector> vec;
                vec.reserve(size);
                for (int j = 0; j < size; j++) {
                    int32_t x, y;
                    stream >> x >> y;
                    vec.push_back(Geometry::Vector(static_cast<int>(x), static_cast<int>(y)));
                }
                newUnits.push_back(std::move(vec));
            }
        }
        
        void Write(OutputMemoryStream &stream, std::vector<std::vector<Geometry::Vector>> &newUnits) {
            stream << static_cast<int32_t>(Message::Process) << static_cast<int32_t>(newUnits.size());
            for (int i = 0; i < newUnits.size(); i++) {
                stream << static_cast<int32_t>(newUnits[i].size());
                for (int j = 0; j < newUnits[i].size(); j++) {
                    stream << static_cast<int32_t>(newUnits[i][j].x) << static_cast<int32_t>(newUnits[i][j].y);
                }
            }
        }
    };
    
}

#endif /* Network_h */

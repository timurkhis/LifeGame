//
//  Network.h
//  LifeGame
//
//  Created by Максим Бакиров on 02.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Network_h
#define Network_h

#include <unordered_map>
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
    
    template <Message type>
    bool IsMsg(InputMemoryStream &stream) {
        int32_t msgType;
        stream.Read(msgType, sizeof(uint32_t));
        return static_cast<Message>(msgType) == type;
    }
    
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
    struct Msg<Message::Init, int, Geometry::Vector, std::unordered_map<int, std::vector<Geometry::Vector>>> {
        void Read(InputMemoryStream &stream, int &player, Geometry::Vector &fieldSize, std::unordered_map<int, std::vector<Geometry::Vector>> &allUnits) {
            uint32_t msgSize;
            int32_t type, number, x, y, units;
            stream >> msgSize >> type >> number >> x >> y >> units;
            if (static_cast<Message>(type) != Message::Init) {
                Log::Error("Messages types are not the same!", type);
            }
            player = static_cast<int>(number);
            fieldSize = Geometry::Vector(static_cast<int>(x), static_cast<int>(y));
            for (int i = 0; i < units; i++) {
                int32_t player, size;
                stream >> player >> size;
                std::vector<Geometry::Vector> vec;
                vec.reserve(size);
                for (int j = 0; j < size; j++) {
                    int32_t x, y;
                    stream >> x >> y;
                    vec.push_back(Geometry::Vector(static_cast<int>(x), static_cast<int>(y)));
                }
                allUnits[static_cast<int>(player)] = std::move(vec);
            }
        }
        
        void Write(OutputMemoryStream &stream, int &player, Geometry::Vector &fieldSize, std::unordered_map<int, std::vector<Geometry::Vector>> &allUnits) {
            uint32_t msgSize;
            stream << msgSize << static_cast<int32_t>(Message::Init) << static_cast<int32_t>(player);
            stream << static_cast<int32_t>(fieldSize.x) << static_cast<int32_t>(fieldSize.y) << static_cast<int32_t>(allUnits.size());
            for (const auto &iter : allUnits) {
                int32_t player = static_cast<int32_t>(iter.first);
                const std::vector<Geometry::Vector> &vec = iter.second;
                stream << player << static_cast<int32_t>(vec.size());
                for (int i = 0; i < vec.size(); i++) {
                    stream << static_cast<int32_t>(vec[i].x) << static_cast<int32_t>(vec[i].y);
                }
            }
            msgSize = stream.Size();
            stream.Write(msgSize, 0);
        }
    };
    
    template <>
    struct Msg<Message::Turn, int, std::vector<Geometry::Vector>> {
        void Read(InputMemoryStream &stream, int &player, std::vector<Geometry::Vector> &units) {
            uint32_t msgSize;
            int32_t type, number, size;
            stream >> msgSize >> type >> number >> size;
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
            uint32_t msgSize;
            stream << msgSize << static_cast<int32_t>(Message::Turn) << static_cast<int32_t>(player) << static_cast<int32_t>(units.size());
            for (int i = 0; i < units.size(); i++) {
                Geometry::Vector vec = units[i];
                stream << static_cast<int32_t>(vec.x) << static_cast<int32_t>(vec.y);
            }
            msgSize = stream.Size();
            stream.Write(msgSize, 0);
        }
    };
    
    template <>
    struct Msg<Message::Process, std::unordered_map<int, std::vector<Geometry::Vector>>> {
        void Read(InputMemoryStream &stream, std::unordered_map<int, std::vector<Geometry::Vector>> &newUnits) {
            uint32_t msgSize;
            int32_t type, units;
            stream >> msgSize >> type >> units;
            if (static_cast<Message>(type) != Message::Process) {
                Log::Error("Messages types are not the same!", type);
            }
            for (int i = 0; i < units; i++) {
                int32_t player, size;
                stream >> player >> size;
                std::vector<Geometry::Vector> vec;
                vec.reserve(size);
                for (int j = 0; j < size; j++) {
                    int32_t x, y;
                    stream >> x >> y;
                    vec.push_back(Geometry::Vector(static_cast<int>(x), static_cast<int>(y)));
                }
                newUnits[static_cast<int>(player)] = std::move(vec);
            }
        }
        
        void Write(OutputMemoryStream &stream, std::unordered_map<int, std::vector<Geometry::Vector>> &newUnits) {
            uint32_t msgSize;
            stream << msgSize << static_cast<int32_t>(Message::Process) << static_cast<int32_t>(newUnits.size());
            for (const auto &iter : newUnits) {
                int32_t player = static_cast<int32_t>(iter.first);
                const std::vector<Geometry::Vector> &vec = iter.second;
                stream << player << static_cast<int32_t>(vec.size());
                for (int i = 0; i < vec.size(); i++) {
                    stream << static_cast<int32_t>(vec[i].x) << static_cast<int32_t>(vec[i].y);
                }
            }
            msgSize = stream.Size();
            stream.Write(msgSize, 0);
        }
    };
    
}

#endif /* Network_h */

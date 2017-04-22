//
//  Utils.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 22.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include <iostream>
#include <sstream>
#include <random>
#include <unordered_map>
#include <errno.h>

class Random {
    static std::mt19937 generator;
    
public:
    static int32_t Next();
    static int32_t Next(int32_t max);
    static int32_t Next(int32_t min, int32_t max);
    static uint32_t NextUInt();
    static float NextFloat();
    static void Seed(uint32_t seed);
};

class Log {
public:
    template <typename ...Args>
    static void Warning(Args... args) {
        std::stringstream stream;
        Message(stream, args...);
        std::cout << stream.str() << std::endl;
    }
    
    template <typename ...Args>
    static void Error(Args... args) {
        std::stringstream stream;
#if defined(_WIN32)
		int error = WSAGetLastError();
#else
		int error = errno;
#endif
		Message(stream, args..., error);
        throw std::runtime_error(stream.str());
    }
    
private:
    template <typename T, typename ...Args>
    static void Message(std::stringstream &stream, T t, Args... args) {
        stream << t << ' ';
        Message(stream, args...);
    }
    
    template <typename T>
    static void Message(std::stringstream &stream, T t) {
        stream << t;
    }
};

class Profile {
    using Clock = std::chrono::high_resolution_clock;
    static std::unordered_map<std::string, double> results;
    static std::string current;
    static Clock::time_point start;
    
public:
    static void Start(const std::string &what);
    static void End();
    static void Print();
};

#endif /* Log_hpp */

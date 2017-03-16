//
//  Log.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 22.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include <iostream>
#include <sstream>
#include <errno.h>

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
    
    template <class Exception, typename ...Args>
    static void Error(Args... args) {
        std::stringstream stream;
        Message(stream, args...);
        throw Exception(stream.str());
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

#endif /* Log_hpp */

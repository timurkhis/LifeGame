//
//  Reporter.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 11.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Reporter_hpp
#define Reporter_hpp

#include <sstream>
#include <stdexcept>
#include <errno.h>

namespace Network {
    
    class NetworkException : public std::runtime_error {
    public:
        NetworkException(const std::string &message) : std::runtime_error(message) {}
    };
    
    class Reporter {
    public:
        template <typename ...Args>
        static void Report(Args... args) {
            std::stringstream stream;
            Message(stream, args...);
            throw NetworkException(stream.str());
        }
        
    private:
        template <typename T, typename ...Args>
        static void Message(std::stringstream &stream, T t, Args... args) {
            stream << t << ' ';
            Message(stream, args...);
        }
        
        template <typename T>
        static void Message(std::stringstream &stream, T t) {
            stream << t << ' ' << errno;
        }
    };
    
}

#endif /* Reporter_hpp */

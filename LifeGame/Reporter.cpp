//
//  Reporter.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 11.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <errno.h>
#include <string>
#include "Reporter.hpp"

namespace Network {
    
    void Reporter::Report(const char *report) {
        std::string error(report);
        throw std::invalid_argument(error + std::to_string(errno));
    }
    
}

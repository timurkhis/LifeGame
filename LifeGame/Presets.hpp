//
//  Presets.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 19.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Presets_hpp
#define Presets_hpp

#include <unordered_map>
#include <vector>
#include <string>
#include "Geometry.h"

class Presets {
    std::unordered_map<unsigned char, std::vector<Geometry::Vector>> presets;
    const std::string path;
    
public:
    explicit Presets(const std::string &path);
    void SaveOnDisk();
    void Save(unsigned char preset, const std::vector<Geometry::Vector> *units);
    const std::vector<Geometry::Vector> *Load(unsigned char preset) const;
};

#endif /* Presets_hpp */

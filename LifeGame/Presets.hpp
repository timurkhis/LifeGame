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
    typedef std::vector<Geometry::Vector> Vectors;
    typedef std::shared_ptr<Vectors> VectorsPtr;
    
    std::unordered_map<unsigned char, VectorsPtr> presets;
    const std::string path;
    
public:
    explicit Presets(const std::string &path);
    void SaveOnDisk();
    void Save(unsigned char preset, const VectorsPtr units);
    const VectorsPtr Load(unsigned char preset) const;
};

#endif /* Presets_hpp */

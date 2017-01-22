//
//  Presets.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 19.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include "Presets.hpp"

using namespace Geometry;

Presets::Presets(const std::string &path) : path(path) {
    std::fstream file;
    file.open(path, std::fstream::in | std::fstream::out);
    std::string str;
    while (std::getline(file, str)) {
//        std::cout << str << std::endl;
        size_t equal = str.find('=');
        if (equal != 1) continue;
        char preset = str[0];
        size_t start = 2;
        size_t end = str.find(' ', start);
        auto iter = presets.insert(std::make_pair(preset, std::make_shared<Vectors>()));
        
        while (end != std::string::npos) {
            Vector result;
            size_t comma = str.find(',', start);
            if (comma == std::string::npos) continue;
            result.x = std::stoi(str.substr(start, comma - 1));
            result.y = std::stoi(str.substr(comma + 1, end - 1));
            iter.first->second->push_back(result);
            start = end + 1;
            end = str.find(' ', start);
        }
    }
    file.close();
}

void Presets::SaveOnDisk() {
    std::fstream file;
    file.open(path, std::fstream::out | std::fstream::trunc);
    for (const auto &iter : presets) {
        const VectorsPtr vector = iter.second;
        if (vector->empty()) continue;
        file << iter.first << '=';
        for (int i = 0; i < vector->size(); i++) {
            auto vec = vector->at(i);
            file << std::to_string(vec.x) << ',' << std::to_string(vec.y) << ' ';
//            std::cout << vector[i].x << " " << vector[i].y << std::endl;
        }
        file << '\n';
    }
    file.close();
}

void Presets::Save(unsigned char preset, const VectorsPtr units) {
    auto result = presets.find(preset);
    VectorsPtr vector;
    if (result == presets.end()) {
        auto inserted = presets.insert(std::make_pair(preset, std::make_shared<Vectors>()));
        if (!inserted.second) return;
        vector = inserted.first->second;
    } else {
        vector = result->second;
        vector->clear();
    }
    vector->assign(units->cbegin(), units->cend());
}

const std::shared_ptr<std::vector<Vector>> Presets::Load(unsigned char preset) const {
    auto result = presets.find(preset);
    if (result == presets.end()) return nullptr;
    return result->second;
}

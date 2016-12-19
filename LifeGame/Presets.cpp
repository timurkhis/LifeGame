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

Presets::Presets(const char *path) : path(path) {
    std::fstream file;
    file.open(path, std::fstream::in);
    std::string str;
    while (std::getline(file, str)) {
        std::cout << str << std::endl;
        size_t equal = str.find('=');
        if (equal != 1 || std::count(str.begin(), str.end(), ',') % 2 != 0) continue;
        char preset = str[0];
        size_t start = 2;
        size_t end = str.find(' ', start);
        std::vector<Vector> vectors;
        
        while (end != std::string::npos) {
            Vector result;
            size_t comma = str.find(',', start);
            if (comma == std::string::npos) continue;
            result.x = std::stoi(str.substr(start, comma - 1));
            result.y = std::stoi(str.substr(comma + 1, end - 1));
            vectors.push_back(result);
            start = end + 1;
            end = str.find(' ', start);
        }
        presets.insert(std::make_pair(preset, vectors));
    }
    file.close();
}

Presets::~Presets() {
    std::fstream file;
    file.open(path, std::fstream::out | std::fstream::trunc);
    for (const auto &iter : presets) {
        const std::vector<Vector> &vector = iter.second;
        if (vector.empty()) continue;
        file << iter.first << '=';
        for (int i = 0; i < vector.size(); i++) {
            file << std::to_string(vector[i].x) << ',' << std::to_string(vector[i].y) << ' ';
        }
        file << '\n';
    }
    file.close();
}

void Presets::Save(char preset, const std::vector<Vector> *units) {
    auto result = presets.find(preset);
    if (result == presets.end()) {
        std::vector<Vector> vector(*units);
        presets.insert(std::make_pair(preset, vector));
    } else {
        result->second.assign(units->cbegin(), units->cend());
    }
}

const std::vector<Vector> *Presets::Load(char preset) const {
    auto result = presets.find(preset);
    if (result == presets.end()) return nullptr;
    return &result->second;
}

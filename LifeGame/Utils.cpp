//
//  Utils.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 22.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Utils.hpp"

std::mt19937 Random::generator = std::mt19937();

int32_t Random::Next() {
    std::uniform_int_distribution<int32_t> distr;
    return distr(generator);
}

int32_t Random::Next(int32_t max) {
    std::uniform_int_distribution<int32_t> distr(0, max);
    return distr(generator);
}

int32_t Random::Next(int32_t min, int32_t max) {
    std::uniform_int_distribution<int32_t> distr(min, max);
    return distr(generator);
}

uint32_t Random::NextUInt() {
    std::uniform_int_distribution<uint32_t> distr;
    return distr(generator);
}

float Random::NextFloat() {
    std::uniform_real_distribution<float> distr;
    return distr(generator);
}

void Random::Seed(uint32_t seed) {
    generator.seed(seed);
}

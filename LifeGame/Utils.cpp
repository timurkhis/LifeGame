//
//  Utils.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 22.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Utils.hpp"

std::mt19937 Random::generator = std::mt19937(std::random_device()());

int32_t Random::Next() {
    std::uniform_int_distribution<int32_t> distr;
    int32_t result = distr(generator);
    return result;
}

int32_t Random::Next(int32_t max) {
    std::uniform_int_distribution<int32_t> distr(0, max);
    int32_t result = distr(generator);
    return result;
}

int32_t Random::Next(int32_t min, int32_t max) {
    std::uniform_int_distribution<int32_t> distr(min, max);
    int32_t result = distr(generator);
    return result;
}

uint32_t Random::NextUInt() {
    std::uniform_int_distribution<uint32_t> distr;
    uint32_t result = distr(generator);
    return result;
}

float Random::NextFloat() {
    std::uniform_real_distribution<float> distr;
    float result = distr(generator);
    return result;
}

void Random::Seed(uint32_t seed) {
    Log::Warning("Seed", seed);
    generator.seed(seed);
}

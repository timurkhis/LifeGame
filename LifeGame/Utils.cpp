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

bool Random::NextBool() {
    return Next() % 2 == 0;
}

void Random::Seed(uint32_t seed) {
    Log::Warning("Seed", seed);
    generator.seed(seed);
}

std::unordered_map<std::string, double> Profile::results = std::unordered_map<std::string, double>();
std::chrono::high_resolution_clock::time_point Profile::start = std::chrono::high_resolution_clock::now();
std::string Profile::current = std::string("");

void Profile::Start(const std::string &what) {
    Clock::time_point end = Clock::now();
    if (current.size() > 0 && what != current) {
        auto pair = results.find(current);
        double duration = Clock::duration(end - start).count();
        if (pair == results.end()) {
            results.emplace_hint(pair, current, duration);
        } else {
            results[current] += duration;
        }
    }
    current = what;
    start = end;
}

void Profile::End() {
    Start("");
}

void Profile::Print() {
    double total = 0.0;
    for (const auto &pair : results) {
        total += pair.second;
    }
    for (const auto &pair : results) {
        std::cout << pair.first << ": " << pair.second << "ticks " << (pair.second / total) * 100 << "%" << std::endl;
    }
}

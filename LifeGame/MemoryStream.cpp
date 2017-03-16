//
//  MemoryStream.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "MemoryStream.hpp"

namespace Network {
    
    MemoryStream::MemoryStream(uint32_t capacity) : size(0), capacity(capacity) {
        buffer = static_cast<uint8_t *>(std::malloc(capacity));
    }
    
    MemoryStream::~MemoryStream() {
        std::free(buffer);
    }
    
    void MemoryStream::Realloc(uint32_t size) {
        void *newPtr = std::realloc(buffer, size);
        if (newPtr == nullptr) {
            throw std::bad_alloc();
        }
        buffer = static_cast<uint8_t *>(newPtr);
        capacity = size;
    }
    
    void MemoryStream::Resize(uint32_t newSize) {
        if (newSize > capacity) {
            Realloc(std::max(capacity * 2, newSize));
        }
        size = newSize;
    }
    
    InputMemoryStream::InputMemoryStream(uint32_t capacity) : MemoryStream(capacity) {}
    
    uint32_t InputMemoryStream::Serialize(void *data, uint32_t bytesCount, uint32_t pos) {
        if (capacity - pos < bytesCount) {
            throw std::out_of_range("Not enough data!");
        }
        std::memcpy(data, buffer + pos, bytesCount);
        return bytesCount;
    }
    
    OutputMemoryStream::OutputMemoryStream(uint32_t capacity) : MemoryStream(capacity) {}
    
    uint32_t OutputMemoryStream::Serialize(void *data, uint32_t bytesCount, uint32_t pos) {
        const uint32_t newSize = pos + bytesCount;
        if (newSize > capacity) {
            Realloc(std::max(capacity * 2, newSize));
        }
        std::memcpy(buffer + pos, data, bytesCount);
        return bytesCount;
    }
    
}

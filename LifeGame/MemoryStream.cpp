//
//  MemoryStream.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "MemoryStream.hpp"

using namespace Geometry;

namespace Network {
    
    MemoryStream::MemoryStream(size_t capacity) : size(0), capacity(capacity) {
        buffer = static_cast<uint8_t *>(std::malloc(capacity));
    }
    
    MemoryStream::~MemoryStream() {
        std::free(buffer);
    }
    
    void MemoryStream::Realloc(size_t size) {
        void *newPtr = std::realloc(buffer, size);
        if (newPtr == nullptr) {
            throw std::bad_alloc();
        }
        buffer = static_cast<uint8_t *>(newPtr);
        capacity = size;
    }
    
    InputMemoryStream::InputMemoryStream(size_t capacity) : MemoryStream(capacity) {}
    
    void InputMemoryStream::Serialize(void *data, uint32_t bytesCount) {
        if (capacity - size < bytesCount) {
            throw std::out_of_range("Not enough data!");
        }
        std::memcpy(data, buffer + size, bytesCount);
        size += bytesCount;
    }
    
    OutputMemoryStream::OutputMemoryStream(size_t capacity) : MemoryStream(capacity) {}
    
    void OutputMemoryStream::Serialize(void *data, uint32_t bytesCount) {
        const size_t newSize = size + bytesCount;
        if (newSize > capacity) {
            Realloc(std::max(capacity * 2, newSize));
        }
        std::memcpy(buffer + size, data, bytesCount);
        size += bytesCount;
    }
    
}

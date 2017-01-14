//
//  MemoryStream.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <stdexcept>
#include "MemoryStream.hpp"

using namespace Geometry;

namespace Network {
    
    MemoryStream::~MemoryStream() {}
    
    InputMemoryStream::InputMemoryStream(size_t size) {
        head = 0;
        Resize(size);
    }
    
    void InputMemoryStream::Serialize(void *data, uint32_t bytesCount) {
        if (Length() < bytesCount) {
            throw std::out_of_range("Not enough data!");
        }
        std::memcpy(data, buffer.data() + head, bytesCount);
        head += bytesCount;
    }
    
    OutputMemoryStream::OutputMemoryStream() {
        head = 0;
    }
    
    void OutputMemoryStream::Serialize(void *data, uint32_t bytesCount) {
        const int capacity = static_cast<int>(buffer.capacity());
        const int newSize = static_cast<int>(head + bytesCount);
        if (newSize > capacity) {
            Resize(std::max(capacity * 2, newSize));
        }
        std::memcpy(buffer.data() + head, data, bytesCount);
        head += bytesCount;
    }
    
}

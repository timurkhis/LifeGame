//
//  MemoryStream.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 12.01.17.
//  Copyright © 2017 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef MemoryStream_hpp
#define MemoryStream_hpp

#include <vector>
#include <netdb.h>
#include <type_traits>
#include <stdint.h>
#include "Vector.hpp"

namespace Network {
    
    template <typename From, typename To>
    class TypeAliaser {
        union {
            From from;
            To to;
        };
        
    public:
        TypeAliaser(From from) : from(from) {}
        constexpr To &Get() const { return to; }
    };
    
    template <typename T, size_t size>
    struct ByteSwapper {};
    
    template <typename T>
    T ByteSwap(T data, bool out) {
        return ByteSwapper<T, sizeof(T)>().Swap(data, out);
    }
    
    template <typename T>
    struct ByteSwapper<T, 2> {
        T Swap(T data, bool out) {
            uint16_t temp = TypeAliaser<T, uint16_t>(data).Get();
            uint16_t result = out ? htons(temp) : ntohs(temp);
            return TypeAliaser<uint16_t, T>(result).Get();
        }
    };
    
    template <typename T>
    struct ByteSwapper<T, 4> {
        T Swap(T data, bool out) {
            uint32_t temp = TypeAliaser<T, uint32_t>(data).Get();
            uint32_t result = out ? htonl(temp) : ntohl(temp);
            return TypeAliaser<uint32_t, T>(result).Get();
        }
    };
    
    template <typename T>
    struct ByteSwapper<T, 8> {
        T Swap(T data, bool out) {
            uint64_t temp = TypeAliaser<T, uint64_t>(data).Get();
            uint64_t result = out ? htonll(temp) : ntohll(temp);
            return TypeAliaser<uint64_t, T>(result).Get();
        }
    };
    
    class MemoryStream {
    private:
        virtual void Serialize(void *data, uint32_t bytesCount) = 0;
        virtual bool IsInput() const = 0;
        
    protected:
        uint8_t *buffer;
        size_t size;
        size_t capacity;
        
    public:
        explicit MemoryStream(size_t capacity);
        virtual ~MemoryStream() = 0;
        
        size_t Capacity() const { return capacity; }
        size_t Size() const { return size; }
        bool Empty() const { return size == 0; }
        void *Data() { return buffer; }
        void Clear() { size = 0; }
        void Realloc(size_t size);
        
        template <typename T>
        void Serialize(T &data) {
            static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Template Serialize only supports primitive data types!");
            if (IsInput()) {
                T temp;
                Serialize(&temp, sizeof(T));
                data = ByteSwap(temp, false);
            } else {
                T swapped = ByteSwap(data, true);
                Serialize(&swapped, sizeof(T));
            }
        }
    };
    
    class InputMemoryStream : public MemoryStream {
    private:
        virtual bool IsInput() const override { return true; }
        virtual void Serialize(void *data, uint32_t bytesCount) override;
        
    public:
        explicit InputMemoryStream(size_t capacity);
        ~InputMemoryStream() override {}
        
        template <typename T>
        friend InputMemoryStream &operator >> (InputMemoryStream &stream, T &data) {
            stream.MemoryStream::Serialize(data);
            return stream;
        }
    };
    
    class OutputMemoryStream : public MemoryStream {
    private:
        virtual bool IsInput() const override { return false; }
        virtual void Serialize(void *data, uint32_t bytesCount) override;
        
    public:
        explicit OutputMemoryStream(size_t capacity);
        ~OutputMemoryStream() override {}
        
        template <typename T>
        friend OutputMemoryStream &operator << (OutputMemoryStream &stream, T data) {
            stream.MemoryStream::Serialize(data);
            return stream;
        }
    };
    
}

#endif /* MemoryStream_hpp */

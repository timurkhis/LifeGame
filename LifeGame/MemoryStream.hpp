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
    struct ByteSwapper<T, 1> {
        T Swap(T data, bool out) {
            return data;
        }
    };
    
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
        virtual uint32_t Serialize(void *data, uint32_t bytesCount, uint32_t pos) = 0;
        virtual bool IsInput() const = 0;
        
    protected:
        uint8_t *buffer;
        uint32_t size;
        uint32_t capacity;
        
    public:
        explicit MemoryStream(uint32_t capacity);
        virtual ~MemoryStream() = 0;
        
        uint32_t Capacity() const { return capacity; }
        uint32_t Size() const { return size; }
        bool Empty() const { return size == 0; }
        void *Data(uint32_t offset = 0) { return buffer + offset; }
        void Clear() { size = 0; }
        void Resize(uint32_t newSize);
        void Realloc(uint32_t size);
        
    protected:
        template <typename T>
        uint32_t Serialize(T &data, uint32_t pos) {
            static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Template Serialize only supports primitive data types!");
            uint32_t result;
            if (IsInput()) {
                T temp;
                result = Serialize(&temp, sizeof(T), pos);
                data = ByteSwap(temp, false);
            } else {
                T swapped = ByteSwap(data, true);
                result = Serialize(&swapped, sizeof(T), pos);
            }
            return result;
        }
    };
    
    class InputMemoryStream : public MemoryStream {
    private:
        virtual bool IsInput() const override { return true; }
        virtual uint32_t Serialize(void *data, uint32_t bytesCount, uint32_t pos) override;
        
    public:
        explicit InputMemoryStream(uint32_t capacity);
        ~InputMemoryStream() override {}
        
        template <typename T>
        void Read(T &data, uint32_t pos) {
            MemoryStream::Serialize(data, pos);
        }
        
        template <typename T>
        friend InputMemoryStream &operator >> (InputMemoryStream &stream, T &data) {
            uint32_t result = stream.MemoryStream::Serialize(data, stream.size);
            stream.size += result;
            return stream;
        }
    };
    
    class OutputMemoryStream : public MemoryStream {
    private:
        virtual bool IsInput() const override { return false; }
        virtual uint32_t Serialize(void *data, uint32_t bytesCount, uint32_t pos) override;
        
    public:
        explicit OutputMemoryStream(uint32_t capacity);
        ~OutputMemoryStream() override {}
        
        template <typename T>
        void Write(T data, uint32_t pos) {
            MemoryStream::Serialize(data, pos);
        }
        
        template <typename T>
        friend OutputMemoryStream &operator << (OutputMemoryStream &stream, T data) {
            uint32_t result = stream.MemoryStream::Serialize(data, stream.size);
            stream.size += result;
            return stream;
        }
    };
    
}

#endif /* MemoryStream_hpp */

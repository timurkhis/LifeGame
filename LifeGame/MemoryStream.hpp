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
    class ByteSwapper {};
    
    template <typename T>
    T ByteSwap(T data, bool out) {
        return ByteSwapper<T, sizeof(T)>().Swap(data, out);
    }
    
    template <typename T>
    class ByteSwapper<T, 2> {
    public:
        T Swap(T data, bool out) {
            uint16_t temp = TypeAliaser<T, uint16_t>(data).Get();
            uint16_t result = out ? htons(temp) : ntohs(temp);
            return TypeAliaser<uint16_t, T>(result).Get();
        }
    };
    
    template <typename T>
    class ByteSwapper<T, 4> {
    public:
        T Swap(T data, bool out) {
            uint32_t temp = TypeAliaser<T, uint32_t>(data).Get();
            uint32_t result = out ? htonl(temp) : ntohl(temp);
            return TypeAliaser<uint32_t, T>(result).Get();
        }
    };
    
    template <typename T>
    class ByteSwapper<T, 8> {
    public:
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
        std::vector<uint8_t> buffer;
        uint32_t head;
        
    public:
        virtual ~MemoryStream() = 0;
        virtual size_t Length() const { return head; }
        void *Data() { return buffer.data(); }
        void Clear() { head = 0; }
        void Resize(size_t size) { buffer.resize(size); }
        
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
        InputMemoryStream(size_t size);
        ~InputMemoryStream() override {}
        
        virtual size_t Length() const override { return buffer.capacity() - head; }
        
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
        OutputMemoryStream();
        ~OutputMemoryStream() override {}
        
        template <typename T>
        friend OutputMemoryStream &operator << (OutputMemoryStream &stream, T &data) {
            stream.MemoryStream::Serialize(data);
            return stream;
        }
    };
    
}

#endif /* MemoryStream_hpp */

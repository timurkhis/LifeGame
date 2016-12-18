//
//  Vector.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Vector_hpp
#define Vector_hpp

#include <functional>

struct Vector {
    int x;
    int y;
    
public:
    explicit Vector() : x(0), y(0) {}
    explicit Vector(int x, int y) : x(x), y(y) {}
    
    static float Dot(const Vector &lhs, const Vector &rhs);
    static Vector Zero() { return Vector(); }
    
    friend const Vector operator - (const Vector &lhs, const Vector &rhs);
    friend const Vector operator + (const Vector &lhs, const Vector &rhs);
    friend Vector &operator -= (Vector &lhs, const Vector &rhs);
    friend Vector &operator += (Vector &lhs, const Vector &rhs);
    friend bool operator == (const Vector &lhs, const Vector &rhs);
    
    friend const Vector operator * (const Vector &lhs, int value);
    friend const Vector operator / (const Vector &lhs, int value);
    friend Vector &operator *= (Vector &lhs, int value);
    friend Vector &operator /= (Vector &lhs, int value);
};

template <>
struct std::hash<Vector> {
    size_t operator () (const Vector &vec) const {
        size_t hash = 17;
        hash = ((hash + vec.x) << 5) - (hash + vec.x);
        hash = ((hash + vec.y) << 5) - (hash + vec.y);
        return hash;
    }
};

#endif /* Vector_hpp */

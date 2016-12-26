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

namespace Geometry {
    
    struct Vector {
        int x;
        int y;
    
    public:
        explicit Vector() : x(0), y(0) {}
        explicit Vector(int x, int y) : x(x), y(y) {}
    
        static int Dot(const Vector &lhs, const Vector &rhs);
        static Vector Zero() { return Vector(); }
        static Vector One() { return Vector(1, 1); }
        static Vector Right() { return Vector(1, 0); }
        static Vector Up() { return Vector(0, 1); }
        
        friend const Vector operator - (const Vector &lhs, const Vector &rhs);
        friend const Vector operator + (const Vector &lhs, const Vector &rhs);
        friend Vector &operator -= (Vector &lhs, const Vector &rhs);
        friend Vector &operator += (Vector &lhs, const Vector &rhs);
        friend bool operator == (const Vector &lhs, const Vector &rhs);
    
        friend const Vector operator * (const Vector &lhs, int value);
        friend const Vector operator / (const Vector &lhs, int value);
        friend Vector &operator *= (Vector &lhs, int value);
        friend Vector &operator /= (Vector &lhs, int value);
    
        friend const Vector operator + (const Vector &self);
        friend const Vector operator - (const Vector &self);
        
        friend std::ostream &operator << (std::ostream &lhs, const Vector &rhs);
    };
    
}

template <>
struct std::hash<Geometry::Vector> {
    size_t operator () (const Geometry::Vector &vec) const {
        size_t hash = 17;
        hash = ((hash + vec.x) << 5) - (hash + vec.x);
        hash = ((hash + vec.y) << 5) - (hash + vec.y);
        return hash;
    }
};

#endif /* Vector_hpp */

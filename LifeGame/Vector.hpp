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
        static const Vector zero;
        static const Vector one;
        static const Vector right;
        static const Vector up;
        
        static int Dot(const Vector &lhs, const Vector &rhs);
        
        explicit Vector() : x(0), y(0) {}
        explicit Vector(int x, int y) : x(x), y(y) {}
        
        friend const Vector operator - (const Vector &lhs, const Vector &rhs);
        friend const Vector operator + (const Vector &lhs, const Vector &rhs);
        friend Vector &operator -= (Vector &lhs, const Vector &rhs);
        friend Vector &operator += (Vector &lhs, const Vector &rhs);
        friend bool operator == (const Vector &lhs, const Vector &rhs);
        friend bool operator != (const Vector &lhs, const Vector &rhs);
        
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

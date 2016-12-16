//
//  Vector.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Vector_hpp
#define Vector_hpp

struct Vector {
    int x;
    int y;
    
public:
    explicit Vector() : x(0), y(0) {}
    explicit Vector(int x, int y) : x(x), y(y) {}
    
    static Vector Zero() { return Vector(); }
    
    friend const Vector operator - (const Vector &lhs, const Vector &rhs);
    friend const Vector operator + (const Vector &lhs, const Vector &rhs);
    friend Vector &operator -= (Vector &lhs, const Vector &rhs);
    friend Vector &operator += (Vector &lhs, const Vector &rhs);
    friend bool operator == (const Vector &lhs, const Vector &rhs);
    
    friend const Vector operator * (const Vector &lhs, int value);
    friend const Vector operator / (const Vector &lhs, int value);
};

#endif /* Vector_hpp */

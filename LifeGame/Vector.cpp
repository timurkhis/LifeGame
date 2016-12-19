//
//  Vector.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Vector.hpp"

int Vector::Dot(const Vector &lhs, const Vector &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

const Vector operator - (const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x - rhs.x, lhs.y - rhs.y);
}

const Vector operator + (const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x + rhs.x, lhs.y + rhs.y);
}

Vector &operator -= (Vector &lhs, const Vector &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

Vector &operator += (Vector &lhs, const Vector &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

bool operator == (const Vector &lhs, const Vector &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

const Vector operator * (const Vector &lhs, int value) {
    return Vector(lhs.x * value, lhs.y * value);
}

const Vector operator / (const Vector &lhs, int value) {
    return Vector(lhs.x / value, lhs.y / value);
}

Vector &operator *= (Vector &lhs, int value) {
    lhs.x *= value;
    lhs.y *= value;
    return lhs;
}

Vector &operator /= (Vector &lhs, int value) {
    lhs.x /= value;
    lhs.y /= value;
    return lhs;
}

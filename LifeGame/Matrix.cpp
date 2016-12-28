//
//  Matrix.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 25.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include <iostream>
#include "Geometry.h"

namespace Geometry {
    
    Matrix3x3 Matrix3x3::Identity() {
        Matrix3x3 result;
        result.m00 = 1.f;
        result.m11 = 1.f;
        result.m22 = 1.f;
        return result;
    }
    
    Matrix3x3 Matrix3x3::Translation(Vector position) {
        Matrix3x3 result = Matrix3x3::Identity();
        result.m02 = position.x;
        result.m12 = position.y;
        return result;
    }
    
    Matrix3x3 Matrix3x3::Scale(Vector scale) {
        Matrix3x3 result = Matrix3x3::Identity();
        result.m00 = scale.x;
        result.m11 = scale.y;
        return result;
    }
    
    Matrix3x3 Matrix3x3::Rotation(float angle) {
        Matrix3x3 result = Matrix3x3::Identity();
        const float radians = angle * DegToRad;
        result.m00 = cosf(radians);
        result.m01 =-sinf(radians);
        result.m10 = sinf(radians);
        result.m11 = cosf(radians);
        return result;
    }
    
    Matrix3x3::Matrix3x3() {
        for (int i = 0; i < size * size; i++) {
            array[i] = 0.f;
        }
    }
    
    const Matrix3x3 operator * (const Matrix3x3 &lhs, const Matrix3x3 &rhs) {
        Matrix3x3 result;
        for (int i = 0; i < Matrix3x3::size; i++) {
            for (int k = 0; k < Matrix3x3::size; k++) {
                for (int j = 0; j < Matrix3x3::size; j++) {
                    result.table[i][j] += lhs.table[i][k] * rhs.table[k][j];
                }
            }
        }
        return result;
    }
    
    const Vector operator * (const Matrix3x3 &lhs, const Vector &rhs) {
        const float x = lhs.m00 * rhs.x + lhs.m01 * rhs.y + lhs.m02 * 1.f;
        const float y = lhs.m10 * rhs.x + lhs.m11 * rhs.y + lhs.m12 * 1.f;
        return Vector(ceilf(x + 0.5f), ceilf(y + 0.5f));
    }
    
    std::ostream &operator << (std::ostream &lhs, const Matrix3x3 &rhs) {
        for (int i = 0; i < rhs.size; i++) {
            for (int j = 0; j < rhs.size; j++) {
                lhs << rhs.table[i][j] << ' ';
            }
            lhs << std::endl;
        }
        return lhs;
    }
    
    void MultiplyMatrices(float *result, const float *lMatrix, const float *rMatrix, size_t lRows, size_t lColumns, size_t rColumns) {
        for (int i = 0; i < lRows; i++) {
            for (int j = 0; j < rColumns; j++) {
                for (int inner = 0; inner < lColumns; inner++) {
                    result[i * rColumns + j] += lMatrix[i * rColumns + inner] * rMatrix[inner * rColumns + j];
                }
            }
        }
    }
    
}

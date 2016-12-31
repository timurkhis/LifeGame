//
//  Matrix.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 25.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Matrix_hpp
#define Matrix_hpp

#include <cstddef>
#include "Vector.hpp"

namespace Geometry {
    
    class Matrix3x3 {
        static const int size = 3;
        union {
            float array[size * size];
            float table[size][size];
            struct {
                float m00;
                float m01;
                float m02;
                float m10;
                float m11;
                float m12;
                float m20;
                float m21;
                float m22;
            };
        };
        
    public:
        static Matrix3x3 Identity();
        static Matrix3x3 Translation(Vector position);
        static Matrix3x3 Scale(Vector scale);
        static Matrix3x3 Rotation(float angle);
        
        explicit Matrix3x3();
        
        friend const Matrix3x3 operator * (const Matrix3x3 &lhs, const Matrix3x3 &rhs);
        friend const Vector operator * (const Matrix3x3 &lhs, const Vector &rhs);
        friend std::ostream &operator << (std::ostream &lhs, const Matrix3x3 &rhs);
    };
    
    //Just for templated fun
    void MultiplyMatrices(float *result, const float *lMatrix, const float *rMatrix, size_t lRows, size_t lColumns, size_t rColumns);
    
    template <size_t rows, size_t columns>
    class Matrix {
        float matrix[rows][columns];
        
    public:
        
        constexpr size_t Rows() const { return rows; }
        constexpr size_t Columns() const { return columns; }
        
        const float *GetPtr() const { return &matrix[0][0]; }
        float *GetPtr() { return &matrix[0][0]; }
        
        static Matrix Identity() {
            static_assert(rows == columns, "Identity matrix should be a square matrix!");
            Matrix result;
            for (int i = 0; i < rows; i++) {
                result.matrix[i][i] = 1.f;
            }
            return result;
        }

        explicit Matrix() {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < columns; j++) {
                    matrix[i][j] = 0.f;
                }
            }
        }
        
        template <size_t r, size_t c>
        friend const Matrix<rows, c> operator * (const Matrix &lhs, const Matrix<r, c> &rhs) {
            static_assert(columns == r, "Multiplication of matrices is possible only if count of columns in lhs equals count of rows in rhs!");
            Matrix<rows, c> result;
            MultiplyMatrices(result.GetPtr(), lhs.GetPtr(), rhs.GetPtr(), lhs.Rows(), lhs.Columns(), rhs.Columns());
            return result;
        }
        
        friend const Vector operator * (const Matrix &lhs, const Vector &rhs) {
            Matrix<3, 1> fromVector;
            float *ptr = fromVector.GetPtr();
            ptr[0] = rhs.x;
            ptr[1] = rhs.y;
            ptr[2] = 1.f;
            Matrix<3, 1> result = lhs * fromVector;
            return Vector(result.GetPtr()[0], result.GetPtr()[1]);
        }
    };
    
}

#endif /* Matrix_hpp */

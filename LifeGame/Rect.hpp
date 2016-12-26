//
//  Rect.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 18.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Rect_hpp
#define Rect_hpp

#include "Vector.hpp"

namespace Geometry {
    
    class Rect {
        Vector min;
        Vector max;
        
    public:
        explicit Rect();
        explicit Rect(Vector position, Vector size);
        
        Vector Center() const { return min + (max - min) / 2; }
        Vector Min() const { return min; }
        Vector Max() const { return max; }
        int MinX() const { return min.x; }
        int MinY() const { return min.y; }
        int MaxX() const { return max.x; }
        int MaxY() const { return max.y; }
        
        bool IsZero() const;
        bool Contains(Vector vec) const;
    };
    
}
#endif /* Rect_hpp */

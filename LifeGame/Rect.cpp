//
//  Rect.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 18.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#include "Rect.hpp"

namespace Geometry {
    
    Rect::Rect() {}

    Rect::Rect(Vector position, Vector size) {
        min = position;
        max = position + size;
    }

    bool Rect::IsZero() const {
        return min == max;
    }

    bool Rect::Contains(Vector vec) const {
        if (vec.x < min.x) return false;
        if (vec.x > max.x) return false;
        if (vec.y < min.y) return false;
        if (vec.y > max.y) return false;
        return true;
    }
    
}

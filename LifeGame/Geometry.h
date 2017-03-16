//
//  Geometry.h
//  LifeGame
//
//  Created by Максим Бакиров on 25.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Geometry_h
#define Geometry_h

#if defined(_WIN32)
#define _USE_MATH_DEFINES
#include <math.h>
#else
#include <cmath>
#endif  

#include "Vector.hpp"
#include "Rect.hpp"
#include "Matrix.hpp"

namespace Geometry {

    const float DegToRad = static_cast<float>(M_PI) / 180.0f;
    const float RadToDeg = 180.0f / static_cast<float>(M_PI);
    
}

#endif /* Geometry_h */

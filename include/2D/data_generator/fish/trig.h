/*
 * Copyright 2012 Cornell Database Group
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRIG
#define TRIG

// get base trig functions from math
#define _USE_MATH_DEFINES
#include <cmath>

// euclid norm
inline float norm(float x, float y) {
    return sqrtf(x*x + y*y);
}

// euclid norm
inline float norm2(float x, float y) {
    return x*x + y*y;
}

// angle (radians)
inline float angle(float x1, float y1, float x2, float y2) {
    float ang = atan2(y2, x2) - atan2(y1, x1);
    return (ang > M_PI)? ang - 2.0f * (float)M_PI : (ang < -M_PI)? 2.0f * (float)M_PI + ang : ang;
}

// rotation
inline void rotate(float &x, float &y, float angle) {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);
    float rX = x * cosAngle - y * sinAngle;
    float rY = x * sinAngle + y * cosAngle;
    x = rX;
    y = rY;
}


#endif


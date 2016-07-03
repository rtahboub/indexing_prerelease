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

/*
 *  Region2D.h
 *  TemplateTests
 *
 *  Created by Alan Demers on 5/12/09.
 *  Copyright 2009 Cornell. All rights reserved.
 *
 */
#ifndef _REGION_2D_FISH_H_
#define _REGION_2D_FISH_H_

#include <cmath>

inline double L2Norm(double x, double y) {
    return sqrt( x*x + y*y );
}

inline double L2Dist( double x0, double y0, double x1, double y1 ) {
    double dx = x1 - x0;  double dy = y1 - y0;  return sqrt( dx*dx + dy*dy );
}

/*
 * A two-dimensional region -- there are multiple kinds.
 *
 * Each kind of region is able to check containment of a point.
 *
 * There are also overloaded functions to test overlap and inclusion
 * and to create bounding regions, e.g. the bounding box for a circle.
 */

struct ORange2D {
    double x0;
    double x1;
    double y0;
    double y1;
    inline bool contains(double x, double y) {
        return (x >= x0) && (x < x1) && (y >= y0) && (y < y1);
    }
    ORange2D(double ax0, double ax1, double ay0, double ay1) : x0(ax0), x1(ax1), y0(ay0), y1(ay1) {}
    ORange2D() { x0 = 0.0;  x1 = 0.0;  y0 = 0.0;  y1 = 0.0; }
    ~ORange2D() {}
};

struct L2Circle2D {
    double x;
    double y;
    double r;
    inline bool contains(double ax, double ay) {
        double dx = ax - x;  double dy = ay - y;
        return r*r > dx*dx + dy*dy;
    }
    L2Circle2D(double ax, double ay, double ar) : x(ax), y(ay), r(ar) {}
    L2Circle2D() : x(0.0), y(0.0), r(0.0) {}
    ~L2Circle2D() {}
};


/*
 * template<typename R1, typename R2> bool overlaps(R1 &r1, R2 &r2)
 *
 * returns true iff regions r1 and r2 have at least one point in common.
 */
template <typename R1, typename R2>
bool overlaps(R1 &r1, R2 &r2);

template<>
inline bool overlaps<ORange2D, ORange2D>(ORange2D &r1, ORange2D &r2) {
    return (r1.x1 > r2.x0) && (r1.x0 < r2.x1) && (r1.y1 > r2.y0) && (r1.y0 < r2.y1);
}

template<>
inline bool overlaps<L2Circle2D, L2Circle2D>(L2Circle2D &r1, L2Circle2D &r2) {
    double dx = r2.x - r1.x;  double dy = r2.y - r1.y; double rsum = r1.r + r2.r;
    return rsum*rsum > dx*dx + dy*dy;
}

/*
 * template <typename R1, typename R2> bool includes(R1 &r1, R2 &r2)
 *
 * returns true iff every point of r2 is in r1
 */
template <typename R1, typename R2>
bool includes(R1 &r1, R2 &r2);

template<>
inline bool includes<ORange2D, ORange2D>(ORange2D &r1, ORange2D &r2) {
    return (r1.x1 >= r2.x1) && (r1.x0 <= r2.x0) && (r1.y1 >= r2.y1) && (r1.y0 <= r2.y0);
}

template<>
inline bool includes<L2Circle2D, L2Circle2D>(L2Circle2D &r1, L2Circle2D &r2) {
    double dx = r2.x - r1.x;  double dy = r2.y - r1.y; double dist = sqrt(dx*dx + dy*dy);
    return r1.r >= (dist + r2.r);
}

/*
 * template <typename R1, typename R2> void boundingRegion(R1 &r1, R2 &r2)
 *
 * sets r1 to a bounding region (of type R1) for region r2
 * so that
 *  boundingRegion<R1, R2>(r1, r2)
 * followed by
 *  includes<R1, R2>(r1, r2)
 * returns true
 */
 
template <typename R1, typename R2>
void boundingRegion(R1 &r1, R2 &r2);

template<>
inline void boundingRegion<ORange2D, L2Circle2D>(ORange2D &r1, L2Circle2D &r2) {
    r1.x0 = r2.x - r2.r;  r1.x1 = r2.x + r2.r;
    r1.y0 = r2.y - r2.r;  r1.y1 = r2.y + r2.r;
}

// TODO: think about points on the boundary?

template<>
inline void boundingRegion<L2Circle2D, ORange2D>(L2Circle2D &r1, ORange2D &r2) {
    r1.x = 0.5 * (r2.x0 + r2.x1);  r1.y = 0.5*(r2.y0 + r2.y1);
    r1.r = 0.5 * L2Dist( r2.x0, r2.y0, r2.x1, r2.y1 );
}

#endif /* _REGION_2D_FISH_ */

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

//
// Everything in this file could well be done with direct C++ structs and none of this pair / tuple stuff.
//

#ifndef UNIT
#define UNIT

#include <utility>
#include <limits>
#include "2D/Region2D.h"

namespace PSimIndex {

/**
 * Units in the fish simulation have an oid, a position vector, a velocity vector, 
 * and a speed intensity.
 */
    class Unit {
    public:
        // action radius (distance)
        const static float ALPHA;
        const static float RHO;

        // angle pertubations (radians)
        const static float GAUSS_MU;
        const static float GAUSS_SIGMA;

        // maximum angle rotation per tick (radians)
        const static float THETA;


        // proportion of informed individuals 
        const static float P;
    
        // strength of individuality (vs. social interactions)
        const static float OMEGA;

        //private:
    public: //TMP
        // these attributes should be generated according to the 
        // specific unit implementation in SGL
        int id;
        float x, y;
        float vx, vy;
        float speed;
        bool informed; 
        float gx, gy;
    
    public:
        Unit() {}

        Unit(int _id, float _x, float _y, float _vx, float _vy, float _speed, bool _informed, float _gx, float _gy) : 
            id(_id), x(_x), y(_y), vx(_vx), vy(_vy), speed(_speed), informed(_informed), gx(_gx), gy(_gy) { }


        Unit(Unit* other) {
            id = other->getOid();
            x = other->getX();
            y = other->getY();
            vx = other->getVx();
            vy = other->getVy();
            speed = other->getSpeed();
            informed = other->getInformed();
            gx = other->getGx();
            gy = other->getGy();
        }

        // accessors for state attributes:
        inline int getOid() const { return id; }
        inline float getX() const { return x; }
        inline float getY() const { return y; }
        inline float getVx() const { return vx; }
        inline float getVy() const { return vy; }
        inline float getSpeed() const { return speed; }
        inline bool getInformed() const { return informed; }
        inline float getGx() const { return gx; }
        inline float getGy() const { return gy; }


        // only state attributes that get updated here:
        inline void setX(const float& newX) { x = newX; }
        inline void setY(const float& newY) { y = newY; }
        inline void setVx(const float& newVx) { vx = newVx; }
        inline void setVy(const float& newVy) { vy = newVy; }
        inline void setGx(const float& newGx) { gx = newGx; }
        inline void setGy(const float& newGy) { gy = newGy; }
    };


    std::ostream& operator<<(std::ostream &out, const Unit& p);

    class UnitKey {
    public:
        typedef float KeyType0;
        typedef float KeyType1;
        typedef int IDType;

        static inline float getKey0(const Unit* const u) {
            return u->getX();
        }

        static inline float getKey1(const Unit* const u) {
            return u->getY();
        }

        static inline int getOID(const Unit* const u) {
            return u->getOid();
        }

        // Multipliy by getSpeed to get accurate answer for TPR-Tree.
        static inline float getV0(const Unit* const u) {
            return u->getVx() * u->getSpeed();
        }

        static inline float getV1(const Unit* const u) {
            return u->getVy() * u->getSpeed();
        }

        static inline double getKey0Min() {
            return std::numeric_limits<float>::min();
        }

        static inline float getKey0Max() {
            return std::numeric_limits<float>::max();
        }

        static inline double getKey1Min() {
            return std::numeric_limits<float>::min();
        }

        static inline double getKey1Max() {
            return std::numeric_limits<float>::max();
        }
    };


    class UnitRG2D {
    public:
        float delta;

        /**
         * TODO: This returns the region by value. Al and I postulated that
         * this is probably okay, but we should check.
         */
        inline Region2D<float, float> getRegion(const Unit* const u) {


        
            float xLo = u->getX() - delta;
            float xHi = u->getX() + delta;
            float yLo = u->getY() - delta;
            float yHi = u->getY() + delta;

            Region2D<float, float> r(xLo, xHi, yLo, yHi);

            return r;
        }

        UnitRG2D(float delta) : delta(delta) {}

    };




/**
 * Represents the effects computed for a given unit.
 * An alternative representation would be to place each effect 
 * in a separate class (UnitEffectNewX, UnitEffectNewY,...).
 *
 * TODO comment from Al:
 * Note that in SGL the type of an effect variable includes its combinator function.
 * We could mimic that here, so we could uniformly say "identity" and "oplus"
 * for all effect variables, if we thought it was beneficial.
 */
    class UnitEffects {
    private:
        // again these attributes should be generated from SGL
        int oid;
        float dX;
        float dY;

    public:
        UnitEffects() : oid(0), dX(0.0), dY(0.0) {} 
        UnitEffects(int id) : oid(id), dX(0.0), dY(0.0) {} 

        inline int getOid() const { return oid; }
        inline void setOid(const int& newOid) { oid = newOid; }

        inline float getDX() const { return dX; }
        inline float getDY() const { return dY; }
    
        inline void clear() {
            dX = 0.0;
            dY = 0.0;
        }

        inline void update(UnitEffects& eff) {
            // suppose all of them were declared with sum
            dX += eff.dX;
            dY += eff.dY;
        }

        inline void updateDX(const float& dXElem) {
            dX += dXElem;
        }

        inline void updateDY(const float& dYElem) {
            dY += dYElem;
        }
    };

    class UnitAgg2D {
    public:
        inline void reset() {}
        inline void combine(const UnitAgg2D* const otherAgg) {}
        inline void combineFromOne(const Unit* const point) {}
        inline float getVal() const {return 0.0;}
        inline bool equals(const UnitAgg2D* const other) const { return true; }
        UnitAgg2D() {}
        UnitAgg2D(UnitAgg2D* other) {}
    };


//struct Goal {
//    float x, y;
//    Goal(float _x, float _y) : x(_x), y(_y) {}
//};


    class Goal {
    private:
        float gx, gy;
    public:
        Goal(float _gx, float _gy) : gx(_gx), gy(_gy) {}
        inline float getGx() const { return gx; }
        inline float getGy() const { return gy; }
    };
}
#endif

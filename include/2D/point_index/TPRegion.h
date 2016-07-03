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


#ifndef _TPREGION_H_
#define _TPREGION_H_

#include <map>
#include <set>
#include <cmath>
#include <vector>
#include <algorithm>
//#include <tr1/unordered_map>
#include <unordered_map>
#include "2D/Region2D.h"

namespace PSimIndex {

    /**
     * TPRegion2D stores a time-parameterized 2D region
     * _pos for position (e.g. static 2D region at the time _refTick)
     * _vel for velocity
     * If static Region2D is to be represented by TPRegion2D,
     * velocity and _refTick do not matter.
     */
    template<typename T, typename S>
    class TPRegion {


    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:
        Region2D<T, S> _pos; // position of moving object or MBR at refTick
        Region2D<T, S> _vel; // vecocity of moving object or MBR
        int _refTick;

        double areaAtRealTick(double dTick) const;

    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------
    public:

        TPRegion() : _refTick(0) {
            _refTick = 0;
        }

        TPRegion(Region2D<T, S>* pos, Region2D<T, S>* vel, int refTick);

        inline int getRefTick() const { return _refTick; }
        inline void setRefTick(int refTick) { _refTick = refTick; }
        inline void setPos(Region2D<T, S> pos) { _pos = pos; }
        inline void setVel(Region2D<T, S> vel) { _vel = vel; }


        void updatePositionToTick(int queryTick);


        /** returns true if the time-parameterized Region satisfies
         *  time-slice query (see Saltenis, et al. 3.2 Querying for formula)
         *
         * queryTick must be no earlier than _refTick
         */
        bool satisfiesQuery(const Region2D<T, S>* const query, int queryTick);

        template<typename U, typename K>
        bool containsPoint(const U* const point, int queryTick) const;

        /**
         * returns integrated area (volume) between time [tick1 .. tick2]
         * pre-condition: _refTick <= tick1 < tick2 must hold
         */
        double integratedArea(int tick1, int tick2) const;


        /**
         * From "Indexing the Positions of Continuously Moving Objects" by
         * Saltenis et al. pp. 6. 
         *
         * TODO: Assuming I can freely cast to double. 
         * TODO: How to handle overflow if tH is very large?
         */
        static bool getIntersectionTime(TPRegion<T,S>& r1, 
                                        TPRegion<T,S>& r2,
                                        int& low, int& high,
                                        int tLow, int tHigh);

        
        double margin() const;
        T getLow0At(int queryTick) const; 
        T getHigh0At(int queryTick) const;
        S getLow1At(int queryTick) const;
        S getHigh1At(int queryTick) const;


        T getLow0P()  const { return _pos.getLow0();  }
        T getHigh0P() const { return _pos.getHigh0(); }
        S getLow1P()  const { return _pos.getLow1();  }
        S getHigh1P() const { return _pos.getHigh1(); }

        T getLow0V()  const { return _vel.getLow0();  }
        T getHigh0V() const { return _vel.getHigh0(); }
        S getLow1V()  const { return _vel.getLow1();  }
        S getHigh1V() const { return _vel.getHigh1(); }


        /** returns static position at the time _refTick as RType */
        Region2D<T, S> getPosRType() const;

        /** returns velocity as RType */
        Region2D<T, S> getVelRType() const;


        /** returns static position at the time queryTick as RType
         * queryTick must be no earlier than _refTick
         */
        Region2D<T, S> getPosRTypeAt(int queryTick) const;



        friend std::ostream& operator<<(std::ostream &out,
                                        const TPRegion<T, S>& r) {
            return out << "TPRegion(refTick = " << r.getRefTick() << ")"
                       << " Pos" << r.getPosRType() 
                       << " Vel" << r.getVelRType();
            //         << " Mem " << r.getPosMem() << " " << r.getVelMem()
            //         << std::endl;
        }


        



    };

    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Implementation Starts Here
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Private Methods
    //------------------------------------------------------------------------

    template<typename T, typename S>
    double TPRegion<T,S>::areaAtRealTick(double dTick) const {
        return Region2D<T, S>(_pos.getLow0()  + dTick * _vel.getLow0(),
                              _pos.getHigh0() + dTick * _vel.getHigh0(),
                              _pos.getLow1()  + dTick * _vel.getLow1(),
                              _pos.getHigh1() + dTick * _vel.getHigh1()).area();
    }

    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------

    template<typename T, typename S>
    TPRegion<T,S>::TPRegion(Region2D<T, S>* pos, 
                            Region2D<T, S>* vel, 
                            int refTick) : _refTick(refTick){
        _pos =  Region2D<T, S>(pos->getLow0(), pos->getHigh0(),
                               pos->getLow1(), pos->getHigh1());
        _vel =  Region2D<T, S>(vel->getLow0(), vel->getHigh0(),
                               vel->getLow1(), vel->getHigh1());
    }

    template<typename T, typename S>
    void TPRegion<T,S>::updatePositionToTick(int queryTick) {
        DBUTL_ASSERT( _refTick <= queryTick );
        if (queryTick == _refTick) return;

        _pos = Region2D<T, S>( 
                _pos.getLow0() + (queryTick - _refTick) * _vel.getLow0(),
                _pos.getHigh0() + (queryTick - _refTick) * _vel.getHigh0(),
                _pos.getLow1()  + (queryTick - _refTick) * _vel.getLow1(),
                _pos.getHigh1() + (queryTick - _refTick) * _vel.getHigh1());

        _refTick = queryTick;
    }

    template<typename T, typename S>
    bool TPRegion<T,S>::satisfiesQuery(const Region2D<T, S>* const query, 
                                       int queryTick) {
        Region2D<T, S> futureMBR = getPosRTypeAt(queryTick);            

#ifdef USE_TPR_EPS
        //std::cout << "in eps" << std::endl;
        return ((query->getLow0()  <= futureMBR.getHigh0() + TPR_EPS)
                && (query->getHigh0() >= futureMBR.getLow0()  - TPR_EPS)
                && (query->getLow1()  <= futureMBR.getHigh1() + TPR_EPS)
                && (query->getHigh1() >= futureMBR.getLow1()  - TPR_EPS));
#else                
        //std::cout << "in no eps" << std::endl;
        return ((query->getLow0()  <= futureMBR.getHigh0())
                && (query->getHigh0() >= futureMBR.getLow0())
                && (query->getLow1()  <= futureMBR.getHigh1())
                && (query->getHigh1() >= futureMBR.getLow1()));
#endif

    }

    template<typename T, typename S>
    template<typename U, typename K>
    bool TPRegion<T,S>::containsPoint(const U* const point, int queryTick) const {
        Region2D<T, S> futureMBR = getPosRTypeAt(queryTick);
        return futureMBR.containsPoint(point);
    }

    template<typename T, typename S>
    double TPRegion<T,S>::integratedArea(int tick1, int tick2) const {
        DBUTL_ASSERT( tick1 < tick2 );
        Region2D<T, S> MBR = getPosRTypeAt(tick1);
        double H = tick2 - tick1;
        double area = 0.0;
            
        double x1 = MBR.getLow0(), x2 = MBR.getHigh0();
        double y1 = MBR.getLow1(), y2 = MBR.getHigh1();
        double vx1 = _vel.getLow0(), vx2 = _vel.getHigh0();
        double vy1 = _vel.getLow1(), vy2 = _vel.getHigh1();

        double term1 = (x2*y2)*H + (x2*vy2 + vx2*y2)*H*H/2. + (vx2*vy2)*H*H*H/3.;
        double term2 = (x1*y2)*H + (x1*vy2 + vx1*y2)*H*H/2. + (vx1*vy2)*H*H*H/3.;
        double term3 = (x2*y1)*H + (x2*vy1 + vx2*y1)*H*H/2. + (vx2*vy1)*H*H*H/3.;
        double term4 = (x1*y1)*H + (x1*vy1 + vx1*y1)*H*H/2. + (vx1*vy1)*H*H*H/3.;
         
        area = term1 - term2 - term3 + term4;
        return area;
    }


    template<typename T, typename S>
    bool TPRegion<T,S>::getIntersectionTime(TPRegion<T,S>& r1, 
                                            TPRegion<T,S>& r2,
                                            int& low, int& high,
                                            int tLow, int tHigh) {


            //Moving rectangles don't intersect in x dimension. 
            if((r2.getLow0At(tLow) > r1.getHigh0At(tLow) 
                && r2.getLow0At(tHigh) > r1.getHigh0At(tHigh))
               || (r2.getHigh0At(tLow) < r1.getLow0At(tLow) 
                   && r2.getHigh0At(tHigh) < r1.getLow0At(tHigh))) {

                return false;
            }
            //Moving rectangles don't intersect in y dimension. 
            if((r2.getLow1At(tLow) > r1.getHigh1At(tLow) 
                && r2.getLow1At(tHigh) > r1.getHigh1At(tHigh))
               || (r2.getHigh1At(tLow) < r1.getLow1At(tLow) 
                   && r2.getHigh1At(tHigh) < r1.getLow1At(tHigh))) {

                return false;
            }


            double delta;
            int lowX, lowY, highX, highY;

            // std::cout << "r2.getLow0At(tLow): " << r2.getLow0At(tLow)
            //           << " r1.getHigh0At(tLow): " << r1.getHigh0At(tLow) << std::endl;
            // std::cout << "r2.getHigh0At(tLow): " << r2.getHigh0At(tLow)
            //           << " r1.getLow0At(tLow): " << r1.getLow0At(tLow) << std::endl;


            // Compute intersection in x dimension. Note that we always take
            // the ceiling of the tick number, since we only want to count
            // ticks at which the rectangles actually intersect. 
            if(r2.getLow0At(tLow) > r1.getHigh0At(tLow)) {

                delta = (r1.getHigh0At(tLow) - r2.getLow0At(tLow)) 
                    / (r2.getLow0V() - r1.getHigh0V());

                lowX = tLow + (int) ceil(delta);
            }
            else if(r2.getHigh0At(tLow) < r1.getLow0At(tLow)) {
                delta = (r1.getLow0At(tLow) - r2.getHigh0At(tLow)) 
                    / (r2.getHigh0V() - r1.getLow0V());

                lowX = tLow  + (int) ceil(delta);
            }
            else {
                lowX = tLow;
            }

            //std::cout << "lowX: " << lowX << std::endl;


            // std::cout << "r2.getLow0At(tHigh): " << r2.getLow0At(tHigh)
            //           << " r1.getHigh0At(tHigh): " << r1.getHigh0At(tHigh) << std::endl;
            // std::cout << "r2.getHigh0At(tHigh): " << r2.getHigh0At(tHigh)
            //           << " r1.getLow0At(tHigh): " << r1.getLow0At(tHigh) << std::endl;



            if(r2.getLow0At(tHigh) > r1.getHigh0At(tHigh)) {
                delta = (r1.getHigh0At(tLow) - r2.getLow0At(tLow))
                    / (r2.getLow0V() - r1.getHigh0V());

                highX = tLow + (int) floor(delta + 1);                
            }
            else if(r2.getHigh0At(tHigh) < r1.getLow0At(tHigh)) {
                
                //std::cout << "r2.getHigh0V: " << r2.getHigh0V() 
                //<< " r1.getLow0V: " << r1.getLow0V() << std::endl;

                delta = (r1.getLow0At(tLow) - r2.getHigh0At(tLow))
                    / (r2.getHigh0V() - r1.getLow0V());

                //std::cout << "delta: " << delta << std::endl;

                highX = tLow + (int) floor(delta + 1);
                //highX = (int) floor(tLow + delta);
            }
            else {
                highX = tHigh;
            }

            //std::cout << "highX: " << highX << std::endl;

            //Compute intersection in y dimension. 

            if(r2.getLow1At(tLow) > r1.getHigh1At(tLow)) {

                delta = (r1.getHigh1At(tLow) - r2.getLow1At(tLow)) 
                    / (r2.getLow1V() - r1.getHigh1V());

                lowY = tLow + (int) ceil(delta);
            }
            else if(r2.getHigh1At(tLow) < r1.getLow1At(tLow)) {
                delta = (r1.getLow1At(tLow) - r2.getHigh1At(tLow)) 
                    / (r2.getHigh1V() - r1.getLow1V());

                lowY = tLow  + (int) ceil(delta);
            }
            else {
                lowY = tLow;
            }

            if(r2.getLow1At(tHigh) > r1.getHigh1At(tHigh)) {
                delta = (r1.getHigh1At(tLow) - r2.getLow1At(tLow))
                    / (r2.getLow1V() - r1.getHigh1V());

                highY = tLow + (int) floor(delta + 1);                
            }
            else if(r2.getHigh1At(tHigh) < r1.getLow1At(tHigh)) {

                delta = (r1.getLow1At(tLow) - r2.getHigh1At(tLow))
                    / (r2.getHigh1V() - r1.getLow1V());

                highY = tLow + (int) floor(delta + 1);
                //highY = floor(tLow + delta);
            }
            else {
                highY = tHigh;
            }

            // std::cout << "lowX: " << lowX << " lowY: "  << lowY
            //           << " highX: " << highX << " highY: " << highY
            //           << std::endl;

            DBUTL_ASSERT((lowX >= tLow));
            DBUTL_ASSERT((lowY >= tLow));
            DBUTL_ASSERT((highX >= lowX));
            DBUTL_ASSERT((highY >= lowY));
            DBUTL_ASSERT((highX <= tHigh));
            DBUTL_ASSERT((highY <= tHigh));

            //If intersection times in the separate dimensions don't overlap,
            //then the rectangles don't intersect. 
            if(highX <= lowY || highY <= lowX) {
                return false;
            }

            low = std::max(lowX, lowY);
            high = std::min(highX, highY);


            //If regions start and stop intersecting at the same tick, then
            //their intersection should not be reported. This differs from the
            //papers because we have a discrete time model. 
            if(low == high) 
                return false;


            //std::cout << "low " << low << " high: " << high << std::endl;
            return true;

    }

        
    template<typename T, typename S>
    double TPRegion<T,S>::margin() const {
        return ((_pos.getHigh0() - _pos.getLow0())
                + (_pos.getHigh1() - _pos.getLow1()));
    }

    template<typename T, typename S>
    T TPRegion<T,S>::getLow0At(int queryTick) const {
        DBUTL_ASSERT( _refTick <= queryTick );
        return _pos.getLow0() + _vel.getLow0() * (queryTick - _refTick); 
    }

    template<typename T, typename S>
    T TPRegion<T,S>::getHigh0At(int queryTick) const {
        DBUTL_ASSERT( _refTick <= queryTick );
        return _pos.getHigh0() + _vel.getHigh0() * (queryTick - _refTick); 
    }

    template<typename T, typename S>
    S TPRegion<T,S>::getLow1At(int queryTick) const {
        DBUTL_ASSERT( _refTick <= queryTick );
        return _pos.getLow1() + _vel.getLow1() * (queryTick - _refTick); 
    }

    template<typename T, typename S>
    S TPRegion<T,S>::getHigh1At(int queryTick) const {
        DBUTL_ASSERT( _refTick <= queryTick );
        return _pos.getHigh1() + _vel.getHigh1() * (queryTick - _refTick); 
    }


    template<typename T, typename S>
    Region2D<T, S> TPRegion<T,S>::getPosRType() const {
        return Region2D<T, S>(_pos.getLow0(), _pos.getHigh0(),
                              _pos.getLow1(), _pos.getHigh1());
    }

    template<typename T, typename S>
    Region2D<T, S> TPRegion<T,S>::getVelRType() const {
        return Region2D<T, S>(_vel.getLow0(), _vel.getHigh0(),
                              _vel.getLow1(), _vel.getHigh1());
    }

    template<typename T, typename S>
    Region2D<T, S> TPRegion<T,S>::getPosRTypeAt(int queryTick) const {
        DBUTL_ASSERT(_refTick <= queryTick);

        Region2D<T, S> r = 
            Region2D<T, S>(_pos.getLow0()  + (queryTick - _refTick) * _vel.getLow0(),
                           _pos.getHigh0() + (queryTick - _refTick) * _vel.getHigh0(),
                           _pos.getLow1()  + (queryTick - _refTick) * _vel.getLow1(),
                           _pos.getHigh1() + (queryTick - _refTick) * _vel.getHigh1());
            return r;
    }
}


#endif /* _TPREGION_2D_H_ */

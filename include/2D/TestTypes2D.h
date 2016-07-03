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

#ifndef _TEST_TYPES_2D_H_
#define _TEST_TYPES_2D_H_

#include <cmath>
#include <iostream>
#include <string>
#include <limits>
//#include <tr1/functional_hash.h>
#include <functional>


#include "2D/Region2D.h"

#include "util/Log.h"

#include "2D/SpatialIndex2D.h"
#include "2D/MovingSpatialIndex2D.h"
#include "2D/StaticSpatialIndex2D.h"
#include "2D/SpatialJoin2D.h"
#include "2D/MovingSpatialJoin2D.h"
#include "2D/StaticSpatialJoin2D.h"
#include "2D/ContinuousMovingJoin2D.h"

// #include "JoinCallBack.h"
// #include "IncrementalCallBack.h"

namespace PSimIndex {
    /**
     * Represents a moving point in 2D space. This is the "point" object we
     * will use for testing.
     */
    class Point2D {
    public:
        int id;
        double x, y;
        double vx, vy;
        double data;
        static int nextID;

        /**
         * Resets id generation so the next id generated will be 0. This
         * should not be used during a single experiment, but it can be called
         * between runs to ensure that the results will be identical.
         */
        static void resetID() { nextID = 0; }


        /**
         * Returns true iff this point is equal to the "other" point. Two
         * points are equal if all of their attributes are equal.
         */
        inline bool equals(const Point2D* const other) const {
            return (id == other->id
                    && x == other->x
                    && y == other->y
                    && vx == other->vx
                    && vy == other->vy
                    && data == other->data);
        }


        /**
         * Returns true iff this point is equal to the "other" point. Two
         * points are equal if all of their attributes are equal.
         */
        inline bool id_equals(const Point2D* const other) const {
            return (id == other->id);
        }


        Point2D() {
            this->x = 0;
            this->y = 0;
            this->vx = 0;
            this->vy = 0;
            this->data = 0;
            this->id = nextID++;
        }

        /**
         * Constructor for a stationary point.
         *
         */
        Point2D(double x, double y, double data) {
            this->x = x;
            this->y = y;
            this->vx = this->vy = 0;
            this->data = data;
            this->id = nextID++;
        }

        /**
         * Constructor for a moving class.
         *
         */
        Point2D(double x, double y, double vx, double vy, double data) {
            this->x = x;
            this->y = y;
            this->vx = vx;
            this->vy = vy;
            this->data = data;
            this->id = nextID++;
        }


        Point2D(int id, double x, double y, double vx,
                double vy, double data) {
            this->x = x;
            this->y = y;
            this->vx = vx;
            this->vy = vy;
            this->data = data;
            this->id = id;
        }


        Point2D(Point2D* other) {
            this->x = other->x;
            this->y = other->y;
            this->vx = other->vx;
            this->vy = other->vy;
            this->data = other->data;
            this->id = other->id;
        }

        Point2D(Point2D& other) {
            this->x = other.x;
            this->y = other.y;
            this->vx = other.vx;
            this->vy = other.vy;
            this->data = other.data;
            this->id = other.id;
        }

        Point2D(int id, double x, double y) {
        	this->id = id;
            this->x = x;
            this->y = y;
        }



    };

    /**
     * Prints a point to the out stream using the format
     *    Point2D(id, x, y, vx, vy, data);
     */
    std::ostream& operator<<(std::ostream &out, const Point2D& p);




    /**
     * This static key class provides access to the attributes of a Point2D
     * object. In can be passed as the K parameter to the SpatialIndex2D
     * or SpatialJoin2D templates.
     */
    class Key2D {
    public:

        /** The type of the first key of a Point2D object. */
        typedef double KeyType0;

        /** The type of the second key of a Point2D object */
        typedef double KeyType1;

        /** The type of the oid of a Point2D object */
        typedef int IDType;

        /**
         * Returns the first key (x value) of point p.
         *
         */
        static inline double getKey0(const Point2D* const p) {
            return p->x;
        }

        /**
         * Returns the second key (y value) of point p.
         */
        static inline double getKey1(const Point2D* const p) {
            return p->y;
        }

        /**
         * Returns the first component (x component) of point p's velocity.
         */
        static inline double getV0(const Point2D* const p) {
            return p->vx;
        }

        /**
         * Returns the second component (y component) of point p's velocity
         */
        static inline double getV1(const Point2D* const p) {
            return p->vy;
        }

        static inline double getKey0Min() {
            return 0.0;
        }

        static inline double getKey0Max() {
            return (double) std::numeric_limits<unsigned int>::max();
        }

        static inline double getKey1Min() {
            return 0.0;
        }

        static inline double getKey1Max() {
            return (double) std::numeric_limits<unsigned int>::max();
        }

        
        static inline unsigned int quantizeKey0(double k) {
            if(k < 0)
                return 0;
                //std::cout << "x k: " << k << std::endl;
            DBUTL_ASSERT((k >= 0));
            
            return (unsigned int) (k + 0.5);
            
        }

        static inline unsigned int quantizeKey1(double k) {
            if(k < 0)
                return 0;
//                std::cout << "y k: " << k << std::endl;
            DBUTL_ASSERT((k >= 0));
            return (unsigned int) (k + 0.5);
        }


        /**
         * Returns a unique identifier for p.
         *
         */
        static inline int getOID(const Point2D* const p) {
            return p->id;
        }

        /**
         * Returns the square of the L2 distance between p1 and p2.
         */
        static inline double distance(const Point2D* const p1,
                                      const Point2D* const p2) {
            return (getKey0(p1) - getKey0(p2))
                 * (getKey0(p1) - getKey0(p2)) +
                   (getKey1(p1) - getKey1(p2))
                 * (getKey1(p1) - getKey1(p2));
        }
    };



    /**
     * Represents a sum aggregate over the data values for a collection of
     * Point2D objects.
     */
    class SumAgg2D {
    private:
        double sum;

    public:

        /**
         * Resests the sum to 0.0.
         */
        inline void reset() {
            sum = 0.0;
        }

        /**
         * Aggregates two SumAgg2D aggregates by adding the respective sums.
         */
        inline void combine(const SumAgg2D* const otherAgg) {
            sum = sum + otherAgg->getVal();
        }

        /**
         * Aggregates the specified point by adding its data value to the
         * current sum.
         */
        inline void combineFromOne(const Point2D* const point) {
            sum = sum + point->data;
        }

        /**
         * Returns the value of the the sum.
         */
        inline double getVal() const {
            return sum;
        }

        inline bool equals(const SumAgg2D* const other) const {
            return (sum == other->getVal());
        }

        /**
         * Default constructor.
         */
        SumAgg2D() {
            reset();
        }

        SumAgg2D(SumAgg2D* other) {
            sum = other->getVal();
        }

    };




    /**
     * Prints a SumAgg2D to the out stream using the format
     *    SumAgg2D(sum);
     */
    std::ostream& operator<<(std::ostream &out, const SumAgg2D& s);


    class RegionGen2D {
    public:
        double xDelta, yDelta;

        /**
         * TODO: This returns the region by value. Al and I postulated that
         * this is probably okay, but we should check.
         */
        inline Region2D<double, double> getRegion(const Point2D* const p) {

            //LOG(DEBUG) << "in getRegion: " << *p << std::endl;
            double max = (double) std::numeric_limits<unsigned int>::max();

            //Truncate values so that they fall inside [0, max_int]. This is
            //important for quantization. 

            // double xLo = (p->x < xDelta) ? 0.0 : p->x - xDelta;
            // double xHi = (p->x + xDelta > max) ? max : p->x + xDelta;
            // double yLo = (p->y < yDelta) ? 0.0 : p->y - yDelta;
            // double yHi = (p->y + yDelta > max) ? max : p->y + yDelta;
            
            // DBUTL_ASSERT(xLo >= 0);
            // DBUTL_ASSERT(yLo >= 0);
            // DBUTL_ASSERT(xHi <= max);
            // DBUTL_ASSERT(yHi <= max);
            

            // Region2D<double, double> r(xLo, xHi, yLo, yHi);

            Region2D<double, double> r(p->x - xDelta,
                                       p->x + xDelta,
                                       p->y - yDelta,
                                       p->y + yDelta);

            return r;
        }


        bool equals(RegionGen2D* other) {
            return (xDelta == other->xDelta && yDelta == other->yDelta);
        }

        RegionGen2D(double xD, double yD) : xDelta(xD), yDelta(yD) {}

        RegionGen2D(RegionGen2D* other) {
            xDelta = other->xDelta;
            yDelta = other->yDelta;
        }

    };

    /**
     * Prints a RegionGen to the out stream using the format
     *    RegionGen2D(x_delta, y_delta);
     */
    std::ostream& operator<<(std::ostream &out, const RegionGen2D& p);

    template <typename U, typename RG>
    class RGCompare {
    public:
        RG* rg;
        // TODO: perhaps make an enum defining whether we compare by lower x or
        //       something else.
        RGCompare(RG* rg) : rg(rg) {}
        bool operator () (const U* p1, const U* p2) {
            Region2D<double, double> r1 = rg->getRegion(p1);
            Region2D<double, double> r2 = rg->getRegion(p2);
            return r1.getLow0() < r2.getLow0();
        }
    };


    // ----------------------------------------------------------------
    // Convience typedefs for common index types. 
    // ----------------------------------------------------------------

    typedef SpatialJoin2D<Point2D, Key2D, 
                          SumAgg2D, RegionGen2D> JoinType;
    typedef MovingSpatialJoin2D<Point2D, Key2D, 
                                SumAgg2D, RegionGen2D> MovingJoinType;
    typedef StaticSpatialJoin2D<Point2D, Key2D, 
                                SumAgg2D, RegionGen2D> StaticJoinType;

    typedef ContinuousMovingJoin2D<Point2D, Key2D, 
                                   SumAgg2D, RegionGen2D> ContJoinType;

    typedef SpatialIndex2D<Point2D, Key2D, SumAgg2D> IndexType;
    typedef MovingSpatialIndex2D<Point2D, Key2D, SumAgg2D> MovingIndexType;
    typedef StaticSpatialIndex2D<Point2D, Key2D, SumAgg2D> StaticIndexType;


    struct JoinMethod {
        union {
            JoinType* join;
            ContJoinType* contJoin;
        };

        bool isCont; 

        const char* getName() {
            if(isCont)
                return contJoin->getInstanceName();
            else
                return join->getInstanceName();
        }
        
        JoinMethod(JoinType* join) {
            isCont = false;
            this->join = join;
        }

        JoinMethod(ContJoinType* contJoin) {
            isCont = true;
            this->contJoin = contJoin;
        }

        ~JoinMethod() {
            if(isCont)
                delete contJoin;
            else
                delete join;
        }
    }; 


    //TODO: Should this be a base class?
    //DO I even need it any more?
    // template<typename K, typename A, typename B>
    // struct CallBackMethod {
    //     union {
    //         JoinCallBack<A,B>* jcb;
    //         IncrementalCallBack<K,A,B>* icb;
    //     };
    //     bool isCont;
        
    //     void clear() {
    //         if(isCont) {
    //             icb->clear();
    //         }
    //         else {
    //             jcb->clear();
    //         }
    //     }

    //     size_t size() {
    //         if(isCont) {
    //             return icb->size();
    //         }
    //         else {
    //             return jcb->size();
    //         }
    //     }


    //     CallBackMethod(JoinCallBack<A,B>* jcb) {
    //         isCont = false;
    //         this->jcb = jcb;
    //     }

    //     CallBackMethod(IncrementalCallBack<K,A,B>* icb) {
    //         isCont = true;
    //         this->icb = icb;
    //     }

    //     ~CallBackMethod() {
    //         if(isCont)
    //             delete icb;
    //         else
    //             delete jcb;
    //     }
    // };

    JoinMethod* getJoinMethod(IndexType* index);
    JoinMethod* getJoinMethod(JoinType* j);
    JoinMethod* getJoinMethod(ContJoinType* cj);

    // template<typename K, typename A, typename B>
    // CallBackMethod<K,A,B>* getCallBackMethod(JoinCallBack<A,B>* jcb) {
    //     return new CallBackMethod<K,A,B>(jcb);
    // }

    // template<typename K, typename A, typename B>
    // CallBackMethod<K,A,B>* getCallBackMethod(IncrementalCallBack<K,A,B>* icb) {
    //     return new CallBackMethod<K,A,B>(icb);
    // }


}

//Overloading hash functions for appropriate types. 
namespace std {
//    namespace tr1 {

        template <>
        struct hash<PSimIndex::Point2D*> 
            : public unary_function<PSimIndex::Point2D*, size_t> {
            size_t operator()(const PSimIndex::Point2D* p) const {
                return (size_t) p->id;
            }
        };


        template<>
        struct hash<PSimIndex::SumAgg2D*>
            : public unary_function<PSimIndex::SumAgg2D*, size_t> {
        private:
//            static std::tr1::hash<double> hshr;
        	static std::hash<double> hshr;
        public:
            size_t operator()(const PSimIndex::SumAgg2D* a) const {
                return hshr(a->getVal());
            }
        };
//    }
}



#endif /* _TEST_TYPES_2D_H_ */


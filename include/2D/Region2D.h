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


#ifndef _REGION_2D_H_
#define _REGION_2D_H_

#include <iostream>
#include <algorithm>

namespace PSimIndex {


    /**
     * Represents a two-dimensional region. 
     *
     * @tparam T The type of the first dimension. Should support assignment
     *           and inequality comparison.  
     * @tparam S The type of the second dimension. Should support assignment
     *           and inequality comparison.
     *
     * @author Ben Sowell
     */
    template<typename T, typename S>
    class Region2D {

    public:

        /**
         * Returns the low value in the first dimension of the specified
         * region.
         *
         * @param  r  The region. 
         * @return  The low value in the first dimension of the specified
         *          region.
         */
        inline static T low0val(Region2D<T,S>& r) { return r.getLow0(); }

        /**
         * Returns the high value in the first dimension of the specified
         * region.
         *
         * @param  r  The region. 
         * @return  The high value in the first dimension of the specified
         *          region.
         */
        inline static T high0val(Region2D<T,S>& r) { return r.getHigh0(); }

        /**
         * Returns the low value in the second dimension of the specified
         * region.
         *
         * @param  r  The region. 
         * @return  The low value in the second dimension of the specified
         *          region.
         */
        inline static S low1val(Region2D<T,S>& r) { return r.getLow1(); }

        /**
         * Returns the high value in the second dimension of the specified
         * region.
         *
         * @param  r  The region. 
         * @return  The high value in the second dimension of the specified
         *          region.
         */
        inline static S high1val(Region2D<T,S>& r) { return r.getHigh1(); }



        /**
         *
         *
         *
         * @tparam  Iter
         * @param  begin
         * @param  end 
         * @return
         */
        template<typename Iter>
        static Region2D<T,S> combine(Iter begin, Iter end) {
             //TODO: What's the right way to handle an empty range?. 
            DBUTL_ASSERT((begin != end));

            T min0, max0;
            S min1, max1;

            Iter it = begin;

            min0 = (*it)->getLow0();
            max0 = (*it)->getHigh0();
            min1 = (*it)->getLow1();
            max1 = (*it)->getHigh1();

            it++;

            for(; it != end; it++) {
                min0 = std::min((*it)->getLow0(), min0);
                max0 = std::max((*it)->getHigh0(), max0);
                min1 = std::min((*it)->getLow1(), min1);
                max1 = std::max((*it)->getHigh1(), max1);
            }

            Region2D<T,S> r(min0, max0, min1, max1);
            return r;
        }
        

    private:
        T _low0, _high0;
        S _low1, _high1;

    public:

        /**
         * Returns the low value in the first dimension of this region. 
         * @return  The low value in the first dimenson of this region.
         */
        inline T getLow0() const { return _low0; }

        /**
         * Returns the high value in the first dimension of this region. 
         * @return  The high value in the first dimenson of this region.
         */
        inline T getHigh0() const { return _high0; }

        /**
         * Returns the low value in the second dimension of this region. 
         * @return  The low value in the second dimenson of this region.
         */
        inline S getLow1() const { return _low1; }

        /**
         * Returns the high value in the second dimension of this region. 
         * @return  The high value in the second dimenson of this region.
         */
        inline S getHigh1() const { return _high1; }


        /**
         * Sets the low value in the first dimension of this region. 
         * @param  val  The new value. 
         */
        inline void setLow0(T val) { _low0 = val; }


        /**
         * Sets the high value in the first dimension of this region. 
         * @param  val  The new value. 
         */
        inline void setHigh0(T val) { _high0 = val; }

        /**
         * Sets the low value in the second dimension of this region. 
         * @param  val  The new value. 
         */
        inline void setLow1(S val) { _low1 = val; }

        /**
         * Sets the high value in the second dimension of this region. 
         * @param  val  The new value. 
         */
        inline void setHigh1(S val) { _high1 = val; }



        /**
         * Tests whether another region is completely contained within this
         * region.
         *
         * @param  The other region
         * @return  True iff this region contains the specified region. 
         */
        inline bool contains(const Region2D<T, S>* const other) const {
            return ((_low0 <= other->getLow0())
                    && (_high0 >= other->getHigh0())
                    && (_low1 <= other->getLow1())
                    && (_high1 >= other->getHigh1()));
        }

        /**
         * Tests whether a point falls within this region. Note that a point
         * on the border of this region is said to be contained within it. 
         *
         * @tparam  U  The point type. 
         * @tparam  K  The key type. 
         * @param  point  The point. 
         * @returns  True iff this region contains the specified point. 
         */
        template<typename U, typename K>
        inline bool containsPoint(const U* const point) const {
            return ((_low0 <= K::getKey0(point))
                    && (_high0 >= K::getKey0(point))
                    && (_low1 <= K::getKey1(point))
                    && (_high1 >= K::getKey1(point)));
        }


        /**
         * Determine whether this region intersects with another region. Note
         * that we assume that regions contain their boundaries, so two
         * regions may intersect by a single point. 
         *
         * @param  other  The other region. 
         * @returns  True iff this region intersects the specified region. 
         */
        inline bool intersects(const Region2D<T, S>* const other) const {
            return ((_low0 <= other->getHigh0())
                    && (_high0 >= other->getLow0())
                    && (_low1 <= other->getHigh1())
                    && (_high1 >= other->getLow1()));
        }

        /**
         * Computes the intersection of this region with another region as a
         * Region2D<T,S> object. 
         *
         * @param  other  The other region. 
         * @return A Region2D object representing the intersection of this
         *         region with the specified region. 
         */
        inline Region2D<T, S> getIntersection(const Region2D<T, S>* const other) {
            T low0, high0;
            S low1, high1;

            T otherLow0 = other->getLow0();
            T otherHigh0 = other->getHigh0();
            S otherLow1 = other->getLow1();
            S otherHigh1 = other->getHigh1();

            if (_low0 < otherLow0) {
                low0 = otherLow0;
            } else {
                low0 = _low0;
            }

            if (_high0 < otherHigh0) {
                high0 = _high0;
            } else {
                high0 = otherHigh0;
            }

            if (_low1 < otherLow1) {
                low1 = otherLow1;
            } else {
                low1 = _low1;
            }

            if (_high1 < otherHigh1) {
                high1 = _high1;
            } else {
                high1 = otherHigh1;
            }

            return Region2D<T, S>(low0, high0, low1, high1);
        }

        /**
         * Returns the smallest region containing both this region and the
         * specified other region.
         * 
         * @param  other  The other region.
         * @return The minimum bounding rectangle containing this region and
         *         other.
         */
        inline Region2D<T, S> combineMBR(const Region2D<T, S>& other) {
            return Region2D<T, S>(std::min(_low0, other.getLow0()),
                         std::max(_high0, other.getHigh0()),
                         std::min(_low1, other.getLow1()),
                         std::max(_high1, other.getHigh1()));
        }

        /**
         * Computes the area enclosed by this region. For this method to
         * function, it must be possible to subtract elements of type S from
         * elements of type T and the result must be a double. 
         *
         * @returns The area enclosed by this region as a double. 
         */
        inline double area() const {
            return ((_low0 - _low1)*(_low0 - _low1)
                    + (_high0 - _high1)*(_high0 - _high1));
        }


        friend std::ostream& operator<<(std::ostream &out,
                                        const Region2D<T, S>& r) {
            return out << "("
                       << (double) r.getLow0() << ", " << (double) r.getHigh0()
                       << ") x ("
                       << (double) r.getLow1() << ", " << (double) r.getHigh1()
                       << ")";
        }

        
        Region2D<T,S>& operator=(const Region2D<T,S>& rhs) {
            if(this != &rhs) {
                _low0 = rhs.getLow0();
                _high0 = rhs.getHigh0();
                _low1 = rhs.getLow1();
                _high1 = rhs.getHigh1();
            }
            return *this;
        }

        Region2D() {}

        Region2D(T low0, T high0, S low1, S high1) {
            _low0 = low0;
            _high0 = high0;
            _low1 = low1;
            _high1 = high1;
        }
    };




}

#endif /* _REGION_2D_H_ */

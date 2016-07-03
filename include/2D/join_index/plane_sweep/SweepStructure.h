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

#ifndef _SWEEP_STRUCTURE_H_
#define _SWEEP_STRUCTURE_H_

#include "2D/Region2D.h"

namespace PSimIndex {

    template <typename U, typename K>
    class SweepStructure {
    public:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        
        virtual const char* getName() = 0;


        /**
         * Inserts a new rectangle into the sweep structure.  Source is the
         * query point that was used to generate the rectangle.
         */
        virtual void insert(RType r, U* source) = 0;

        /**
         * Removes all rectangles with a right side that falls to the left of
         * the x coordinate of limit.
         */
        virtual void removeExpired(U* limit) = 0;

        /**
         * Reports all rectangles that contain point. They should be passed to
         * the callback in the order source, point.
         */
        virtual void reportOverlap(JoinCallBack<K,U,U>* cb, U* point) = 0;


        /**
         *
         * Used by StripedSweep.
         */
        virtual void setMinMax(typename K::KeyType1 minY, 
                               typename K::KeyType1 maxY) = 0;



        virtual int getSize() = 0;
        

        virtual void print() = 0;

        /**
         * Removes all rectangles from the sweep structure. 
         */
        virtual void clear() = 0;
    };
}


#endif /* _SWEEP_STRUCTURE_H_ */

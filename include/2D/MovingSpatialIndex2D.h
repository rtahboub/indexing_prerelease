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

#ifndef _MOVING_SPATIAL_INDEX_2D_H_
#define _MOVING_SPATIAL_INDEX_2D_H_

#include "2D/SpatialIndex2D.h"


namespace PSimIndex {
/**
 * Represents a spatial index specifically designed to index moving
 * points. These indices typically implicitly maintain a point's position by
 * storing its velocity along with an initial starting position. 
 */
    template<typename U, typename K, typename A>
    class MovingSpatialIndex2D : public SpatialIndex2D<U, K, A> {
    public:
        inline bool isMovingIndex() { return true; }

        /**
         * Updates the velocity of the specified point. This method will be
         * called before the velocity has been updated in the master copy. The
         * new position and velocity are speciifed . 
         */
        virtual void beforeUpdateVelocity(U* point, 
                                          typename K::KeyType0 new0, 
                                          typename K::KeyType1 new1, 
                                          typename K::KeyType0 newV0, 
                                          typename K::KeyType1 newV1)  = 0;

        //virtual void setWorkload(Workload2D* w) {}
        
    };


}


#endif /* _MOVING_SPATIAL_INDEX_2D_H_ */

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

#ifndef _MOVING_SPATIAL_JOIN_2D_H_
#define _MOVING_SPATIAL_JOIN_2D_H_

#include "SpatialJoin2D.h"

namespace PSimIndex {

    template <typename U, typename K, typename A, typename RG>
    class MovingSpatialJoin2D : public SpatialJoin2D<U, K, A, RG> {
    public:        
        inline bool isMovingJoin() { return true; }

        virtual void beforeUpdateVelocity(U* point, 
                                          typename K::KeyType0 new0, 
                                          typename K::KeyType1 new1, 
                                          typename K::KeyType0 newV0, 
                                          typename K::KeyType1 newV1)  = 0;

//        virtual void setWorkload(Workload2D* w) {}
        
        virtual ~MovingSpatialJoin2D() {}
    };
}

#endif /* _MOVING_SPATIAL_JOIN_2D_H_ */

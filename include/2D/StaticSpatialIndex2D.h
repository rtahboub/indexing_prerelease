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

#ifndef _STATIC_SPATIAL_INDEX_2D_H_
#define _STATIC_SPATIAL_INDEX_2D_H_

#include "2D/SpatialIndex2D.h"
//#include "2D/Workload2D.h"

namespace PSimIndex {

    /**
     * Represents a spatial index that deals only with the position of points
     * and does not explicitly handle velocity. The index must be explicitly
     * updated each tick to reflect each point's new location. 
     */
    template<typename U, typename K, typename A>
    class StaticSpatialIndex2D : public SpatialIndex2D<U, K, A> {
    public:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        inline bool isMovingIndex() { return false; }

        /**
         * Default implementation. 
         */
        virtual void aggregateOverRegion(A* aggs, RType* r) {
            IndexCallBack<U> cb;
            this->enumerateOverRegion(&cb,r);
            
            aggs->reset();

            typename IndexCallBack<U>::iterator it;
            for(it = cb.begin(); it != cb.end(); it++) {
                aggs->combineFromOne(*it);
            }
        }

        //      virtual void setWorkload(Workload2D* w) {}
        
        virtual ~StaticSpatialIndex2D() {}
    };
}


#endif /* _STATIC_SPATIAL_INDEX_2D_H_ */

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

#ifndef _SPATIAL_INDEX_2D_H_
#define _SPATIAL_INDEX_2D_H_

#include <vector>
#include "util/pstdint.h"
#include "Region2D.h"
#include "IndexCallBack.h"
//#include "2D/Workload2D.h"

namespace PSimIndex {

    /**
     * Represents a spatial point index that supports range, knn, and
     * aggregate queries over a collection a points. This interface is
     * templated to support a wide variety of base types the template
     * parameters are described below:
     *
     *
     * U is the agent (or point) class. It has no particular requirements.
     *
     *
     * K is the keys class. It should contain the following methods and type
     * declarations:
     *
     *     typedef [type] KeyType0
     *     typedef [type] KeyType1
     *     typedef [type] IDType
     *
     *     static inline KeyType0 getKey0(const U* const u);
     *     static inline KeyType1 getKey1(const U* const u);
     *
     *     // returns a unique identifier for the agent.
     *     static inline IDType getOID(const U* const u);
     *    
     *     // Only necessary for indices that implement nearestK.
     *     // TODO: Can this be double, or is there a reason we want to support
     *     // non-metric spaces?
     *     static inline double distance(const U* const u1, const U* const u2)
     *
     *     // Returns min and max values for key types. 
     *     static inline KeyType0 getKey0Min();
     *     static inline KeyType0 getKey0Max();
     *     static inline KeyType1 getKey1Min();
     *     static inline KeyType1 getKey2Max();
     *
     * Additionally, each key type needs the <, >, and == operators defined.
     *
     * A is the aggregate class. It should contain the following methods:
     *
     *     // Set aggregates to idempotent values.
     *     inline void reset()
     *    
     *     // The o-plus operation. 
     *     inline void combine(A* otherAgg)
     *
     *     // The o-plus operation from a single agent. 
     *     inline void combineFromOne(U* agent)
     *
     * This class must also have a public default constructor. 
     */

    //TUAN DELETE ME
    //  #include "2D/Workload2D.h"

    template <typename U, typename K, typename A>
    class SpatialIndex2D {

    public:

        //TUAN
//        virtual void setWorkload(Workload2D* w) = 0;

        /**
         * Every index should implement the static method getClassName that
         * returns the general name of the strategy.  This cannot be enforced
         * programmatically because we can't have static virutal methods.
         */
        //static const char* getClassName();

        /**
         * Returns the namae of this index, including any instance-specific
         * attributes. 
         */
        virtual const char* getInstanceName() = 0;

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        /**
         * Returns true iff this index is an instance of MovingSpatialIndex2D. 
         */
        virtual bool isMovingIndex() = 0;


        /**
         * Initializes the index on the given set of agents at the beginning
         * of a tick. This will rebuild the index if necessary. This method
         * MUST be called at the beginning of every tick, or the index is not
         * guaranteed to be correct.
         */
        virtual void startTick(std::vector<U*>* agents) = 0;

        /**
         * Enumerates the result of a range query over the specified p
         * region. Callbacks will be made to the specified IndexCallBack
         * object. The order of callbacks is undefined.
         */
        virtual void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) = 0;

        /**
         * Computes an aggregate over a range query. After calling this
         * method, the aggs argument will be set to the aggregates over the
         * region r.
         */
        virtual void aggregateOverRegion(A* aggs, RType* r) = 0;

        /**
         * Enumerates the k nearest neighbors of the specified agent. The
         * agents will be enumerated in closest to farthest order. 
         */
        virtual void nearestK(IndexCallBack<U>* cb, U* u, int k) = 0;

        /**
         * This method notifies the index that an in-place update has been
         * made to the Unit u, but the keys have not been changed.
         * (i.e. K.getKeyX(u) is the same for X=0,1). This
         * method should be called after the the update has been made to
         * the master copy of the unit.
         *
         * Updates that change the keys of the unit should be done as a
         * delete followed by an insert. 
         *
         * Note that once the update is performed, the index may return
         * partial or incorrect results until the next tick. 
         */
        virtual void afterUpdateAgg(U* u) = 0;

        /**
         * Notifies the index that the Unit u should be inserted into the
         * index. This method should be called after the unit has been
         * inserted into the master copy of the units (i.e. u should
         * remain a valid pointer). 
         *
         * Note that once insert is performed, the index may return
         * partial results until the next tick.
         */
        virtual void afterInsert(U* u) = 0;

        /**
         * Notifies the index that the unit with id oid will be
         * deleted. This method needs to be called before the delete takes
         * place in order to ensure that the unit can be found using the
         * existing keys. 
         *
         * Note that once insert is performed, the index may return
         * invalid results until the next tick.
         */
        virtual void beforeDelete(U* u) = 0;

        /**
         * 
         * This method MUST be called at the end of every tick.  If force is
         * true, the index will release all of its resources.
         */
        virtual void endTick(bool force) = 0;

        virtual ~SpatialIndex2D() {}
    };
}

#endif /* _SPATIAL_INDEX_2D_H_ */

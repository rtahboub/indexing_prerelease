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

#ifndef _SPATIAL_JOIN_2D_H_
#define _SPATIAL_JOIN_2D_H_

#include "JoinCallBack.h"
#include <vector>



namespace PSimIndex {

    /**
     * Represents a spatial join strategy. At a high level, each strategy
     * indexes a set of points, and each query specifies a set of points. The
     * result of the query is then a set of pairs containing a query point and
     * a point from the index satisfying the query. For example, an
     * enumeration join returns those points in range of each query point. 
     *
     * The template parameters here are similar to those in SpatialIndex2D:
     *
     * U is the agent (or point) class. 
     *
     * K is the keys class. It has the same requirements as in SpatialIndex2D.
     *
     * A is the aggregate class. It also has the same requirements as in
     * SpatialIndex2D.
     *
     * RG is the region generator class. This class must have the method.
     *    Region2D<K::KeyType0, K::KeyType1> getRegion(U*)
     * This method will be called on each query point (in the agents vector)
     * to recover the appropriate query region. Note that the result is
     * returned by value, not as a pointer. 
     */


    template <typename U, typename K, typename A, typename RG>
    class SpatialJoin2D {
    public:


        /**
         * Returns true iff this index is an instance of MovingSpatialIndex2D. 
         */
        virtual bool isMovingJoin() = 0;

        /**
         * Every index should implement the static method getClassName that
         * returns the general name of the strategy.  This cannot be enforced
         * programmatically because we can't have static virutal methods.
         */
        //static const char* getClassName();

        /**
         * Returns the name of this join strategy. 
         */
        virtual const char* getInstanceName() = 0;

        /**
         * Builds the index if necessary and does any other necessary
         * initialization at the beginning of each tick.
         */
        virtual void startTick(std::vector<U*>* agents) = 0;

        /**
         * Answers an enumeration query. For each query point q in agents, finds
         * the matching points in the index that fall within rg->getRegion(q). 
         *
         * For each point p that joins with the query point q, calls
         * cb->reportPair() on q and p. 
         */
        virtual void enumerationJoin(JoinCallBack<K, U, U>* cb,
                                     std::vector<U*>* agents, RG* rg) = 0;



        /**
         * Answers an aggregate query. For each query point q in agents,
         * computes the aggregate of all of the matching points that fall
         * within the region rg->getRegion(q). These should be reported by
         * calling cb->reportPair() on the query point and its aggregate. 
         */
        virtual void aggregateJoin(JoinCallBack<K, U, A>* cb, 
                                   std::vector<U*>* agents, RG* rg) = 0;


        /**
         * Answers a knn query. For each query point q in agents, reports the
         * k nearest points from the index in order of increasing
         * distance. Answers are reported using cb->reportPair(). 
         */
        /**
         * sowell: Currently unimplemented in all methods. May be useful for
         * future studies.
         */
        virtual void nearestKJoin(JoinCallBack<K, U, U>* cb, 
                                  std::vector<U*>* agents, 
                                  int k) = 0;



        /**
         * Updates the aggregate (data) value of a point. This method will be
         * called after the aggregate has been modified in the master copy of
         * the points. Updates to key attributes should be made by deleting
         * the point and then reinserting it. 
         */
        virtual void afterUpdateAgg(U* u) = 0;


        /**
         * Inserts a point into the index. This method will be called after u
         * has been inserted into the master copy of the points. 
         */
        virtual void afterInsert(U* u) = 0;


        /**
         * Deletes a point from the index. This method will be called before u
         * has been deleted from the master copy of the points. 
         */
        virtual void beforeDelete(U* u) = 0;


        /**
         * Performs any end-of-tick cleanup. If force is true, the index must
         * free all memory it holds. If updates are batched, they should be
         * performed here.
         */
        virtual void endTick(bool force) = 0;

        virtual ~SpatialJoin2D() {}
    };
}

#endif /* _SPATIAL_JOIN_2D_H_ */

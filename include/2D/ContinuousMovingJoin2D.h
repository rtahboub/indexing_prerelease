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

#ifndef _CONTINUOUS_MOVING_JOIN_2D_H_
#define _CONTINUOUS_MOVING_JOIN_2D_H_

#include <vector>
#include "IncrementalCallBack.h"
namespace PSimIndex {

    template<typename U, typename K, typename A, typename RG>
    class ContinuousMovingJoin2D {
    public:

        /**
         * Returns the name of this join strategy. 
         */
        virtual const char* getInstanceName() = 0;
        
        /**
         * Returns the region generator object used by this join instance to 
         * generate queries. 
         */
        virtual RG* getRegionGen() const = 0;

        /**
         * Sets the region generator class used by this join instance to 
         * generate queries. This can be used to update the join to support 
         * newly inserted query points. 
         */
        virtual void setRegionGen(RG* rg) = 0;
        

        virtual IncrementalCallBack<K,U,U>* getCallBack() const = 0;
        virtual void setCallBack(IncrementalCallBack<K,U,U>* icb) = 0;

        /**
         * Initializes the points and queries for the a new tick. The exact 
         * behavior of this method will depend on the implementation. 
         */
        virtual void startTick(std::vector<U*>* agents, 
                               std::vector<U*>* queryAgents) = 0;
        
        /**
         * Updates the result of the join based on updates made during the 
         * last tick. This method only reports the incremental changes to the 
         * join results. The IncrementalCallBack object contains the following 
         * methods
         *     addResult(U* queryPt, U* dataPt)
         *     removeResult(U* queryPt, U* dataPt)
         */
        virtual void updateJoinResults(bool tmp = false/*IncrementalCallBack<U,U>* cb*/) = 0;
                    
        /**
         * Inserts a data point to be joined during the next tick. 
         */
        virtual void insertPoint(U* u) = 0;

        /**
         * Deletes a data point before the next tick. 
         */
        virtual void deletePoint(U* u) = 0;        

        /**
         * Inserts a query point to be joined during the next tick. This point 
         * will be passed to the region generator object to generate a query 
         * rectangle. 
         */
        virtual void insertQuery(U* u) = 0;

        /**
         * Deletes a query point before the next tick. 
         */
        virtual void deleteQuery(U* u) = 0;
        



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
        // virtual void afterInsert(U* u) = 0;

        /**
         * Deletes a point from both the data and query points. 
         */
        virtual void beforeDelete(U* u) = 0;

        /**
         * Updates the velocity of the specified point. Note that this method 
         * applies to both data points and query points. So if a single point 
         * is used as both a data point and a query point, its velocity should 
         * only be updated once. This prevents inconsistencies from arising. 
         */         
        virtual void updateVelocity(U* point, 
                                    typename K::KeyType0 new0, 
                                    typename K::KeyType1 new1, 
                                    typename K::KeyType0 newV0, 
                                    typename K::KeyType1 newV1) = 0;
        
       /**
        * Performs any end-of-tick cleanup. If force is true, the index must
        * free all memory it holds. If updates are batched, they should be
        * performed here.
        */
        virtual void endTick(bool force = false) = 0;

        virtual ~ContinuousMovingJoin2D() {}
    };
}

#endif /* _CONTINUOUS_MOVING_JOIN_2D_H_ */

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

#ifndef _GENERIC_PLANE_SWEEP2D_H_
#define _GENERIC_PLANE_SWEEP2D_H_

#include "2D/join_index/plane_sweep/QuerySort.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"
#include "2D/join_index/plane_sweep/ListSweep.h"
#include "2D/join_index/plane_sweep/StripedSweep.h"

#include "2D/StaticSpatialJoin2D.h"
#include "2D/IndexUtils2D.h"
#include "2D/Region2D.h"
#include "IndexLog.h"

#include "util/Log.h"

#include <stdexcept>
#include <algorithm>
#include <utility>
#include <iostream>

namespace PSimIndex {

    template<typename U, typename K, typename A, typename RG>
    class GenericPlaneSweep2D : public StaticSpatialJoin2D<U, K, A, RG> {

    public:

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        /**
         * Precondition: queryBegin <--> should be sorted by the left side of
         * the sorted region. 
         */
        template<typename DataIterator, typename QueryIterator>
        inline static void genericPlaneSweep(JoinCallBack<K,U,U>* cb,
                                             DataIterator dataBegin,
                                             DataIterator dataEnd,
                                             QueryIterator queryBegin,
                                             QueryIterator queryEnd,
                                             SweepStructure<U,K>* sweepStruct,
                                             RG* rg) {
            
            sweepStruct->clear();

            DataIterator dIt = dataBegin;
            QueryIterator qIt = queryBegin;
            int queryPtr = 0;

            while(dIt != dataEnd) {
                
                U* topAgent = *dIt; //myAgents->at(dataPtr);

                
                DBUTL_ASSERT((topAgent != NULL));
                

                // We have read through all of the queries. All that needs to
                // be done is to join the remaining points with the rectangles
                // in the sweep structure. 
                if(qIt == queryEnd) {
                    break;
                }
                
                U* topSource = *qIt;
                RType topQuery = rg->getRegion(topSource);

                if(topSource == NULL || K::getKey0(topAgent) < topQuery.getLow0()) {
                    sweepStruct->removeExpired(topAgent);
                    sweepStruct->reportOverlap(cb, topAgent);
                    // sweepStruct->print();
                    dIt++;
                }
                else {
                    sweepStruct->insert(topQuery, topSource);
                    queryPtr++;
                    qIt++;
                }
            }

            // Join remaining points with sweepStructure. 
            while(dIt != dataEnd && sweepStruct->getSize() > 0) {
                U* topAgent = *dIt;
                sweepStruct->removeExpired(topAgent);
                sweepStruct->reportOverlap(cb, topAgent);
                dIt++;
            }

            sweepStruct->clear();
        }



    private:
        std::vector<U*>* myAgents;
        SweepStructure<U,K>* sweepStructure;

        char name[200];

        static bool pairlow0(std::pair<RType, U*>* p1, 
                             std::pair<RType, U*>* p2) {
            return rltlow0(p1->first, p2->first);
        }

    public:

        static const char* getClassName() { return "GenericPlaneSweep2D"; }
        const char* getInstanceName() { return name; }

        /**
         * Builds the index if necessary and does any other necessary
         * initialization at the beginning of each tick.
         */
        void startTick(std::vector<U*>* agents) {
            myAgents = new std::vector<U*>(*agents);

            std::sort(myAgents->begin(), myAgents->end(), dim0lt<U,K>);

            typename K::KeyType1 minY = K::getKey1Max();
            typename K::KeyType1 maxY = K::getKey1Min();

            for(int i = 0; i < myAgents->size(); i++) {
                typename K::KeyType1 key = K::getKey1(myAgents->at(i));

                if(key < minY) {
                    minY = key;
                }

                if(key > maxY) {
                    maxY = key;
                }
            }

//            std::cout << "minY: " << minY << " maxY: " << maxY << std::endl;
            sweepStructure->setMinMax(minY - 1.0, maxY + 1.0);

        }

        /**
         * Answers an enumeration query. For each query point q in agents,
         * finds the matching points in the index that fall within
         * rg->getRegion(q).
         *
         * For each point p that joins with the query point q, calls
         * cb->reportPair() on q and p. 
         */
        void enumerationJoin(JoinCallBack<K,U,U>* cb, 
                             std::vector<U*>* queryAgents, 
                             RG* rg) {
            

            std::vector<U*>* myQueryAgents = new std::vector<U*>(*queryAgents);
                
//             int numAgents = myAgents->size();
            int numQueries = myQueryAgents->size();

            RType* queries = new RType[numQueries];
            
            for(int i = 0; i < numQueries; i++) {
                queries[i] = rg->getRegion(myQueryAgents->at(i));
            }

            QuerySort<U,RType,typename K::KeyType0,RType::low0val>
                ::quicksort(queries, myQueryAgents);

            genericPlaneSweep(cb,
                              myAgents->begin(),
                              myAgents->end(),
                              myQueryAgents->begin(),
                              myQueryAgents->end(),
                              sweepStructure,
                              rg);

            delete [] queries;
            delete myQueryAgents;
        }


        /**
         * Answers an aggregate query. For each query point q in agents,
         * computes the aggregate of all of the matching points that fall
         * within the region rg->getRegion(q). These should be reported by
         * calling cb->reportPair() on the query point and its aggregate. 
         */
        void aggregateJoin(JoinCallBack<K,U,A>* cb,
                           std::vector<U*>* agents, 
                           RG* rg) {
            //NOT IMPLEMENTED
            return;
        }

        /**
         * Answers a knn query. For each query point q in agents, reports the
         * k nearest points from the index in order of increasing
         * distance. Answers are reported using cb->reportPair(). 
         */
        void nearestKJoin(JoinCallBack<K,U,U>* cb, 
                          std::vector<U*>* agents, 
                          int k) {
            //NOT IMPLEMENTED
            return;
        }


        /**
         * The plane-sweep algorithm does not process updates. It rejoins the
         * queries and points at every tick.
         */
        void afterUpdateAgg(U* u) {}
        void afterInsert(U* u) {}
        void beforeDelete(U* u) {}


        /**
         * Performs any end-of-tick cleanup. If force is true, the index must
         * free all memory it holds. If updates are batched, they should be
         * performed here.
         */
        void endTick(bool force = false) {
            delete myAgents;
            myAgents = NULL;

            sweepStructure->clear();
        }

        GenericPlaneSweep2D() {
            myAgents = NULL;
            sweepStructure = NULL;

            //Default.
            //sweepStructure = new ListSweep<U,K>();
            sweepStructure = new StripedSweep<U,K>(128);

            sprintf(name, "PlaneSweep%s", sweepStructure->getName());
        }

        GenericPlaneSweep2D(SweepStructure<U,K>* sweepStructure) {
            myAgents = NULL;
            this->sweepStructure = sweepStructure;
            sprintf(name, "PlaneSweep%s", sweepStructure->getName());
        }


        ~GenericPlaneSweep2D() {
            if(myAgents != NULL) {
                delete myAgents;
            }
            if(sweepStructure != NULL){
                delete sweepStructure;
            }
        }
    };
}

#endif /* _GENERIC_PLANE_SWEEP2D_H_ */

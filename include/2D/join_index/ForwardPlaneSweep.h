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

#ifndef _FORWARD_PLANE_SWEEP_H_
#define _FORWARD_PLANE_SWEEP_H_



#include "2D/StaticSpatialJoin2D.h"
#include "2D/IndexUtils2D.h"
#include "2D/Region2D.h"
#include "IndexLog.h"

#include <stdexcept>
#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>

namespace PSimIndex {

    template<typename U, typename K, typename A, typename RG>
    class ForwardPlaneSweep : public StaticSpatialJoin2D<U, K, A, RG> {



    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        std::vector<U*>* myAgents;

        //TODO: Share with GenericPlaneSweep2D
        static bool pairlow0(std::pair<RType, U*>* p1, 
                             std::pair<RType, U*>* p2) {
            return rltlow0(p1->first, p2->first);
        }

    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------
    public:

        /**
         * Performs the forward plane sweep join. This is done in a static
         * method so that we can reuse the code for PBSM etc.
         *
         * NOTE: This method sorts using the passed DataIterator and
         * QueryIterator objects directly. If the underlying collection needs
         * to remain ordered, it should be copied before calling this method. 
         */
        template<typename DataIterator, typename QueryIterator>
        static void forwardPlaneSweep(JoinCallBack<K,U,U>* cb, 
                                      DataIterator dataBegin,
                                      DataIterator dataEnd,
                                      QueryIterator queryBegin,
                                      QueryIterator queryEnd,
                                      RG* rg); 


        static const char* getClassName() { return "ForwardPlaneSweep"; }
        const char* getInstanceName() { return getClassName(); }

        ForwardPlaneSweep() : myAgents(NULL) {}
        ~ForwardPlaneSweep();


        /**
         * Builds the index if necessary and does any other necessary
         * initialization at the beginninig of each tick.
         */
        void startTick(std::vector<U*>* agents);

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
                             RG* rg);

        /**
         * Performs any end-of-tick cleanup. If force is true, the index must
         * free all memory it holds. If updates are batched, they should be
         * performed here.
         */
        void endTick(bool force = false);


        //Unimplemented methods. 
        void aggregateJoin(JoinCallBack<K,U,A>* cb,
                           std::vector<U*>* agents, 
                           RG* rg) {}
        void nearestKJoin(JoinCallBack<K,U,U>* cb, 
                          std::vector<U*>* agents, 
                          int k) {}
        void afterUpdateAgg(U* u) {}
        void afterInsert(U* u) {}
        void beforeDelete(U* u) {}




    };




    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Implementation Starts Here
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------


    template<typename U, typename K, typename A, typename RG>
    template<typename DataIterator, typename QueryIterator>
    void ForwardPlaneSweep<U,K,A,RG>::forwardPlaneSweep(JoinCallBack<K,U,U>* cb, 
                                                        DataIterator dataBegin,
                                                        DataIterator dataEnd, 
                                                        QueryIterator queryBegin,
                                                        QueryIterator queryEnd, 
                                                        RG* rg) {
            
        std::sort(queryBegin, queryEnd, dim0lt<U,K>);
        U* agent;
        typename K::KeyType1 key1;            

        DataIterator dIt = dataBegin;
        //TODO: Is it safe to access queryBegin after sorting? It seems to
        //work, but I don't know if it is guaranteed to...
        QueryIterator qIt = queryBegin;

        DataIterator dIt2;
        QueryIterator qIt2;

        while(dIt != dataEnd && qIt != queryEnd) {
            agent = *dIt;

            DBUTL_ASSERT((agent != NULL));

            U* queryPoint = *qIt;
            RType topQuery = rg->getRegion(queryPoint);

            // Update: It turns out if we do read foward in the query
            // array, then we don't have to sort the query regions at the
            // beginning of method. This is what Nick did in PBSM2D. 
            if(K::getKey0(agent) < topQuery.getLow0()) {

                for(qIt2 = qIt; qIt2 != queryEnd; qIt2++) {
                    queryPoint = *qIt2;
                    topQuery = rg->getRegion(queryPoint);

                    if(topQuery.getLow0() > K::getKey0(agent)) {
                        break;
                    }

                    key1 = K::getKey1(agent);
                    
                    if(K::getOID(queryPoint) != K::getOID(agent)
                       && key1 >= topQuery.getLow1() 
                       && key1 <= topQuery.getHigh1()) {
                        cb->reportPair(queryPoint, agent);
                    }
                }
                dIt++;
            }
            else {
                for(dIt2 = dIt; dIt2 != dataEnd; dIt2++) {                    
                    agent = *dIt2;
                        
                    if(K::getKey0(agent) > topQuery.getHigh0()) {
                        break;
                    }
                    
                    key1 = K::getKey1(agent);
                        
                    if(K::getOID(queryPoint) != K::getOID(agent)
                       && key1 >= topQuery.getLow1() 
                       && key1 <= topQuery.getHigh1()) {
                        cb->reportPair(queryPoint, agent);
                    }
                }                        
                qIt++;
            }
        }
    }

    template<typename U, typename K, typename A, typename RG>
    ForwardPlaneSweep<U,K,A,RG>::~ForwardPlaneSweep() {
        if(myAgents != NULL) {
            delete myAgents;
        }
    }

    template<typename U, typename K, typename A, typename RG>
    void ForwardPlaneSweep<U,K,A,RG>::startTick(std::vector<U*>* agents) {
        myAgents = new std::vector<U*>(*agents);
        std::sort(myAgents->begin(), myAgents->end(), dim0lt<U,K>);
    }


    template<typename U, typename K, typename A, typename RG>
    void ForwardPlaneSweep<U,K,A,RG>::enumerationJoin(JoinCallBack<K,U,U>* cb, 
                                                      std::vector<U*>* qAgents,
                                                      RG* rg) {
            int numAgents = myAgents->size();
            int numQueries = qAgents->size();

            std::vector<U*>* myQAgents = new std::vector<U*>(*qAgents);

            forwardPlaneSweep(cb, 
                              myAgents->begin(), 
                              myAgents->end(), 
                              myQAgents->begin(), 
                              myQAgents->end(), 
                              rg);

            delete myQAgents;
        }

    template<typename U, typename K, typename A, typename RG>
    void ForwardPlaneSweep<U,K,A,RG>::endTick(bool force) {
        delete myAgents;
        myAgents = NULL;
    }

}

#endif /* _FORWARD_PLANE_SWEEP_H_ */

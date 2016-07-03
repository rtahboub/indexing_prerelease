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

#ifndef _BINARY_SEARCH_2D_H_
#define _BINARY_SEARCH_2D_H_

#include "2D/StaticSpatialIndex2D.h"
#include "2D/IndexUtils2D.h"
#include "IndexLog.h"

#include <iostream>

namespace PSimIndex {


    /**
     *
     * @author Ben Sowell
     *
     */
    template<typename U, typename K, typename A>
    class BinarySearch2D : public StaticSpatialIndex2D<U, K, A> {

    private:
        std::vector<U*>* theAgents;
        bool agentsUpdated;

    public:
        static const char* getClassName() { return "BinarySearch2D"; }
        const char* getInstanceName() { return getClassName(); }

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;


        /**
         *
         */
        void startTick(std::vector<U*>* agents) {

            LOG(DEBUG) << "Ben: Starting tick" << std::endl;

            if(theAgents == 0) {
                LOG(DEBUG) << "Ben: HERE" << std::endl;
                theAgents = new std::vector<U*>(*agents);
                std::sort(theAgents->begin(), theAgents->end(), dim0lt<U,K>);
                agentsUpdated = false;
            }

            LOG(DEBUG) << "POINTS" << std::endl;
            for(int i = 0; i < theAgents->size(); i++) {
                LOG(DEBUG) << "BinarySearch2D: " << *theAgents->at(i) << std::endl;
            }
            LOG(DEBUG)  << std::endl;            

        }


        /**
         *
         */
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {

//            std::cout << "processing query " << *r << std::endl;

            int i = 0; 
            int j = theAgents->size();

            // Search for first match. 
            while(i < (j-1)) {
                int m = (i + j) / 2;

                if(K::getKey0(((*theAgents)[m])) < r->getLow0()) { 
                    i = m; 
                } 
                else { 
                    j = m; 
                }
            }

            while(true) {
                //If there are no agents that match on the first
                //dimension, then no more points will match. 
                if(i >= theAgents->size())
                    break;

                U* a = (*theAgents)[i];

//                std::cout << "querying " << *a << " ";


                if(K::getKey0(a) > r->getHigh0()) {
//                    std::cout << "breaking" << std::endl;
                    break;

                }
                else if(K::getKey0(a) >= r->getLow0() 
                        && K::getKey1(a) >= r->getLow1()
                        && K::getKey1(a) <= r->getHigh1()) {
                    cb->reportPoint(a);
//                    std::cout << "reporting" << std::endl;
                }

//                if(i >0)
//                    std::cout << " previous " << *(*theAgents)[i-1];


//                std::cout << std::endl;
                i++;
            }
        }


        /**
         *
         */
        void aggregateOverRegion(A* aggs, RType* r) {

            int i = 0; 
            int j = theAgents->size();

            aggs->reset();

            // Search for first match. 
            while(i < (j-1)) {
                int m = (i + j) / 2;

                if(K::getKey0(((*theAgents)[m])) < r->getLow0()) { 
                    i = m; 
                } 
                else { 
                    j = m; 
                }
            }

            //If there are no agents that match on the first
            //dimension, then no more points will match. 
            while(true) {
                if(i >= theAgents->size())
                    break;

                U* a = (*theAgents)[i];
            
                if(K::getKey0(a) >= r->getHigh0())
                    break;
                else if(K::getKey0(a) > r->getLow0() 
                        && K::getKey1(a) > r->getLow1()
                        && K::getKey1(a) < r->getHigh1()) {
                    aggs->combineFromOne(a);
                }
                i++;
            }
        }

        /**
         * TODO: Improve over the naive implemenation.
         */
        void nearestK(IndexCallBack<U>* cb, U* u, int k) {
            std::vector<U*>* topK = new std::vector<U*>(k, (U*) 0);
            
            typename std::vector<U*>::iterator a, it;

            for(a = theAgents->begin(); a != theAgents->end(); a++) {
                for(it = topK->begin(); it != topK->end(); it++) {
                    if((*it) == 0 || closer<U,K>(u, (*a), (*it))) {
                        topK->insert(it, (*a));
                        topK->pop_back();
                        break;
                    }
                }
            }
            
            for(it = topK->begin(); it != topK->end(); it++) {
                if((*it) != 0)
                    cb->reportPoint(*it);
            }

            delete topK;
        }


        /**
         * Aggregates are computed naively, so we don't need to update the
         * agents
         */
        void afterUpdateAgg(U* u) {
            LOG(DEBUG) << "Ben: UpdateAgg" << std::endl;

            return;
        }

        void afterInsert(U* u) {
            LOG(DEBUG) << "Ben: afterInsert" << std::endl;
            agentsUpdated = true;
        }

        void beforeDelete(U* u) {
            LOG(DEBUG) << "Ben: beforeDelete" << std::endl;
            agentsUpdated = true;
        }

        void endTick(bool force = false) {


            if(force || agentsUpdated) {
                LOG(DEBUG) << "Ben: endTick force" << std::endl;
                delete theAgents;
                theAgents = 0;
            }
            else {
                LOG(DEBUG) << "Ben: endTick no force" << std::endl;
            }

        }

        BinarySearch2D() {
            theAgents = 0;
        }
    };
}

#endif /* _BINARY_SEARCH_2D_H_ */

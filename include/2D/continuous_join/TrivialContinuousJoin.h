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

#ifndef _TRIVIAL_CONTINUOUS_JOIN_H_
#define _TRIVIAL_CONTINUOUS_JOIN_H_

#include <vector>
#include "2D/ContinuousMovingJoin2D.h"

namespace PSimIndex {

    template<typename U, typename K, typename A, typename RG>
    class TrivialContinuousJoin : public ContinuousMovingJoin2D<U,K,A,RG> {
    private:
        RG* rg;
        std::vector<U*>* agents;
        std::vector<U*>* queryAgents;
        IncrementalCallBack<K,U,U>* icb;

    public:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        static const char* getClassName() { return "TrivialContinuousJoin"; }
        const char* getInstanceName() { return getClassName(); }
        
        /**
         * Gets and sets the region generator object used by the this index.  
         */
        RG* getRegionGen() const { return rg; }
        void setRegionGen(RG* rg) { this->rg = rg; }

        virtual IncrementalCallBack<K,U,U>* getCallBack() const { return icb; }
        virtual void setCallBack(IncrementalCallBack<K,U,U>* icb) { 
            this->icb = icb;
        }

        /**
         * Initializes the points and queries for the a new tick. The exact 
         * behavior of this method will depend on the implementation. 
         */
        void startTick(std::vector<U*>* agents, 
                       std::vector<U*>* queryAgents) {
            
            this->agents = agents;
            this->queryAgents = queryAgents;
        }
        
        /**
         * Updates the result of the join based on updates made during the 
         * last tick. This method only reports the incremental changes to the 
         * join results. The IncrementalCallBack object contains the following 
         * methods
         *     addResult(U* queryPt, U* dataPt)
         *     removeResult(U* queryPt, U* dataPt)
         */
        void updateJoinResults(bool tmp = false/*IncrementalCallBack<U,U>* cb*/) {
            typename std::vector<U*>::const_iterator dIt, qIt;

            icb->clear();

            // if(agents == NULL) {
            //     std::cout << "agents is NULL" << std::endl;
            // }

            if(queryAgents == NULL) {
                std::cout << "queryAgents is NULL" << std::endl;
                return;
            }
//            std::cout << "Using RegionGen: " << *rg << std::endl;
          
            //std::cout << "queryAgents size: " << queryAgents->size() << std::endl;
            int count = 0;
            for(qIt = queryAgents->begin(); qIt != queryAgents->end(); qIt++) {
                RType r = rg->getRegion(*qIt);
                for(dIt = agents->begin(); dIt != agents->end(); dIt++) {

//                    std::cout << "HERE " << **qIt << " " << **dIt << " " << r << std::endl;

                    if(K::getOID(*qIt) != K::getOID(*dIt)
                       && r.template containsPoint<U,K>(*dIt)) {
                        
                        //std::cout << "ADDING" << std::endl;

                        icb->reportPair(*qIt, *dIt);
                    }
                }
            }
        }
                    
        /**
         * Trivial implementation is trivial... Join is recomputed completely
         * at each tick, so there is no need to update. 
         */
        void insertPoint(U* u) {}
        void deletePoint(U* u) {}
        void insertQuery(U* u) {}
        void deleteQuery(U* u) {}
        void beforeDelete(U* u) {}
        void afterUpdateAgg(U* u) {}
        void updateVelocity(U* point, 
                            typename K::KeyType0 new0, 
                            typename K::KeyType1 new1, 
                            typename K::KeyType0 newV0, 
                            typename K::KeyType1 newV1) {}
        
       /**
        * Performs any end-of-tick cleanup. If force is true, the index must
        * free all memory it holds. If updates are batched, they should be
        * performed here.
        */
        void endTick(bool force) {

        }
        
        TrivialContinuousJoin() {}
        ~TrivialContinuousJoin() {
//            delete rg;
        }
    };
}

#endif /* _CONTINUOUS_MOVING_JOIN_2D_H_ */

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

#ifndef _STATIC_NESTED_LOOPS_2D_H_
#define _STATIC_NESTED_LOOPS_2D_H_

#include "2D/StaticSpatialIndex2D.h"
#include "2D/StaticSpatialJoin2D.h"
#include "2D/Workload2D.h"
#include "IndexLog.h"

#include <iostream>
#include <cstdio>


namespace PSimIndex {

    template <typename U, typename K, typename A, typename RG>
    class StaticNestedLoops2D : public StaticSpatialJoin2D<U, K, A, RG> {

    private:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        StaticSpatialIndex2D<U,K,A>* index;

        char name[200];

    public:
        //TUAN: DELETE ME
        void setWorkload(Workload2D* w) {        
            index->setWorkload(w);
        }

        static const char* getClassName() { return "StaticNestedLoops2D"; }
        const char* getInstanceName() { return name; };


        void startTick(std::vector<U*>* agents) {
            index->startTick(agents);
        }

        void enumerationJoin(JoinCallBack<K,U,U>* cb,
                             std::vector<U*>* agents,
                             RG* rg) {


        //	std::cout << "HERE here" << std::endl;

            //std::cout << "HERE" << std::endl;

            typename std::vector<U*>::const_iterator it;
            typename IndexCallBack<U>::iterator iit;


            // nb: I expected this to be faster, but it was not. 
            // IndexCallBack<U> icb(cb);

            // for(it = agents->begin(); it != agents->end(); it++) {
            //     icb.left = *it;
            //     RType r = rg->getRegion(*it);
            //     index->enumerateOverRegion(&icb, &r);
            // }


            IndexCallBack<U> icb;
            std::cout<< "agents size = " << agents->size() <<"\n";

            for(it = agents->begin(); it != agents->end(); it++) {

                RType r = rg->getRegion(*it);

                index->enumerateOverRegion(&icb, &r);

//                for(iit = icb.begin(); iit != icb.end(); iit++) {
//                    if(K::getOID(*it) != K::getOID(*iit)) {
//                        LOG(DEBUG) << (**it) << " " << (**iit) << std::endl;
//                        cb->reportPair(*it, *iit);
//                    }
//                }
                for(iit = icb.begin(); iit != icb.end(); iit++) {
                   // if(K::getOID(*it) != K::getOID(*iit)) {
                        LOG(DEBUG) << (**it) << " " << (**iit) << std::endl;
                       // std::cout << (**it) << " " << (**iit) << std::endl;

                        cb->reportPair(*it, *iit);
                 //   }
                }
                icb.clear();
            }


        }


        void aggregateJoin(JoinCallBack<K,U,A>* cb,
                           std::vector<U*>* agents,
                           RG* rg) {

            typename std::vector<U*>::const_iterator it;
            A* a;
            for(it = agents->begin(); it != agents->end(); it++) {
                RType r = rg->getRegion(*it);
                a = new A();
                index->aggregateOverRegion(a, &r);
                cb->reportPair(*it, a);
            }
        }



        void nearestKJoin(JoinCallBack<K,U,U>* cb,
                          std::vector<U*>* agents,
                          int k) {

            typename std::vector<U*>::const_iterator it;
            typename IndexCallBack<U>::iterator iit;

            IndexCallBack<U> icb;

            for(it = agents->begin(); it != agents->end(); it++) {
                index->nearestK(&icb, *it, k);

                for(iit = icb.begin(); iit != icb.end(); it++) {
                    cb->reportPair(*it, *iit);
                }
                icb.clear();
            }
        }


        /** Same as in SpatialIndex2D.h */
        void afterUpdateAgg(U* u) {
            index->afterUpdateAgg(u);
        }

        void afterInsert(U* u) {
            index->afterInsert(u);
        }

        void beforeDelete(U* u) {
            index->beforeDelete(u);
        }

        void endTick(bool force) {
            index->endTick(force);
        }

        StaticNestedLoops2D(StaticSpatialIndex2D<U,K,A>* index) {
            this->index = index;
            sprintf(name, "%sSNL", index->getInstanceName());
        }

        ~StaticNestedLoops2D() {
            delete index;
        }



    };

}
#endif /* _STATIC_NESTED_LOOPS_2D_H_ */


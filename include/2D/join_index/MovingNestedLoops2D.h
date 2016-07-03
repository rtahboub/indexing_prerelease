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

#ifndef _MOVING_NESTED_LOOPS_2D_H_
#define _MOVING_NESTED_LOOPS_2D_H_


#include "2D/MovingSpatialIndex2D.h"
#include "2D/MovingSpatialJoin2D.h"

#include "2D/point_index/Stripes.h"
namespace PSimIndex {

    template <typename U, typename K, typename A, typename RG>
    class MovingNestedLoops2D : public MovingSpatialJoin2D<U, K, A, RG> {

    private:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;        
        MovingSpatialIndex2D<U,K,A>* index;
        int tickNum; 
        char name[200];

    public:

        static const char* getClassName() { return "MovingNestedLoops2D"; }
        const char* getInstanceName() { return name; }

        void startTick(std::vector<U*>* agents) {
            index->startTick(agents);
        }

        void enumerationJoin(JoinCallBack<K,U,U>* cb,
                             std::vector<U*>* agents, 
                             RG* rg) {

            typename std::vector<U*>::const_iterator it;
            typename IndexCallBack<U>::iterator iit;

            IndexCallBack<U> icb;

            for(it = agents->begin(); it != agents->end(); it++) {
                U* u = *it;

                if(K::getOID(u) == 8107 && tickNum == 4) {
                    Stripes<U,K,A>::toPrint = true;
                    //index->toPrint = true;
                }
                else if(tickNum == 4) {
                    Stripes<U,K,A>::toPrint = false;
                }


                RType r = rg->getRegion(*it);
                index->enumerateOverRegion(&icb, &r);

                for(iit = icb.begin(); iit != icb.end(); iit++) {
                    if(K::getOID(*it) != K::getOID(*iit)) {
                        cb->reportPair(*it, *iit);

//                        LOG(DEBUG) << "Tuan: reporting pair " << **it << " " 
//                                   << **iit << std::endl;
                        
                    }
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

        void beforeUpdateVelocity(U* point, 
                                  typename K::KeyType0 new0, 
                                  typename K::KeyType1 new1, 
                                  typename K::KeyType0 newV0, 
                                  typename K::KeyType0 newV1) {
            index->beforeUpdateVelocity(point, new0, new1, newV0, newV1);
        }


        void endTick(bool force) {
            index->endTick(force);
            tickNum++;

            if(force) {
                tickNum = 0;
            }
        }

        MovingNestedLoops2D(MovingSpatialIndex2D<U,K,A>* index) {
            this->index = index;
            tickNum = 0;
            sprintf(name, "%sMNL2D", index->getInstanceName());
        }

        ~MovingNestedLoops2D() {
            delete index;
        }

    };
    
}

#endif /* _MOVING_NESTED_LOOPS_2D_H_ */

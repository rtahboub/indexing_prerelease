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


#ifndef _NAIVE_SPATIAL_INDEX_2D_H_
#define _NAIVE_SPATIAL_INDEX_2D_H_

#include <cstdio>
#include <vector>
#include "2D/StaticSpatialIndex2D.h"
#include "2D/IndexUtils2D.h"

namespace PSimIndex {

    /**
     * Naive algorithm that reports points in a region using a simple linear
     * scan.
     *
     * @author Ben Sowell
     */
    template <typename U, typename K, typename A>
    class NaiveSpatialIndex2D : public StaticSpatialIndex2D<U, K, A> {
    public:

        static const char* getClassName() { return "NaiveSpatialIndex2D"; }
        const char* getInstanceName() { return getClassName(); }

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        void startTick(std::vector<U*>* agents) {
            this->agents = agents;
        }

        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {
            typename std::vector<U*>::const_iterator a;

            for(a = agents->begin(); a != agents->end(); a++) {
                if(r->template containsPoint<U,K>(*a)) {
                    cb->reportPoint(*a);
                }
            }
        }


        void aggregateOverRegion(A* aggs, RType* r) {
            aggs->reset();
            typename std::vector<U*>::const_iterator a;

            for(a = agents->begin(); a != agents->end(); a++) {
                if(r->template containsPoint<U,K>(*a)) {
                    aggs->combineFromOne(*a);
                }
            }
        }

        void nearestK(IndexCallBack<U>* cb, U* u, int k) {
            std::vector<U*>* topK = new std::vector<U*>(k, (U*) 0);

            typename std::vector<U*>::iterator a, it;

            for(a = agents->begin(); a != agents->end(); a++) {
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
         * There is no work to be done during the "build" phase, so these can
         * be noops.
         */
        void afterUpdateAgg(U* u) {}
        void afterInsert(U* u) {}
        void beforeDelete(U* u) {}


        void endTick(bool force = false) {
            agents = 0;
        }

        NaiveSpatialIndex2D() {}
        ~NaiveSpatialIndex2D() {}

    private:
        std::vector<U*>* agents;
   };
}

#endif /* _NAIVE_SPATIAL_INDEX_2D_H_ */

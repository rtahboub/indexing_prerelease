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


#ifndef _NAIVE_MOVING_INDEX_2D_H_
#define _NAIVE_MOVING_INDEX_2D_H_

#include "2D/MovingSpatialIndex2D.h"
#include <vector>

namespace PSimIndex {

    template<typename U, typename K, typename A>
    class NaiveMovingIndex2D : public MovingSpatialIndex2D<U, K, A> {
    private:
        std::vector<U*>* agents;
        bool updated;


//        void updatePos() {
//            typename std::vector<U*>::iterator it;
//            for(it = agents->begin(); it != agents->end(); it++) {
//                Point2D* p = (*it);
//                p->x += p->vx;
//                p->y += p->vy;
//            }
//            updated = true;
//        }



    public:

        static const char* getClassName() { return "NaiveMovingIndex2D"; }
        const char* getInstanceName() { return getClassName(); }

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;


        /**
         * Builds the index if necessary and does any other necessary
         * initialization at the beginning of each tick.
         */
        void startTick(std::vector<U*>* agents) {
            updated = false;
            if(this->agents == NULL) {
                // Copy pointers.  
                this->agents = new std::vector<U*>(*agents);
            }

            typename std::vector<U*>::const_iterator a;
            

            // LOG(DEBUG) << "POINTS" << std::endl;
            // for(a = this->agents->begin(); a != this->agents->end(); a++) {
            //     LOG(DEBUG) << **a << std::endl;
            // }


        }
        /**
         * Answers an enumeration query. Calls cb->reportPoint() on each point
         * contained in region r. 
         */
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {
            typename std::vector<U*>::const_iterator a;

            // LOG(DEBUG) << std::endl;
            // LOG(DEBUG) << "Query: " << *r << std::endl;


            for(a = agents->begin(); a != agents->end(); a++) {
                // LOG(DEBUG) << "Testing point " <<  **a;
                if(r->template containsPoint<U,K>(*a)) {

                    // LOG(DEBUG) << " reporting" << std::endl;
                    
                    cb->reportPoint(*a);
//                    LOG(DEBUG) << "Tuan: reporting point: " << **a << std::endl;
                }
                // else {
                //     LOG(DEBUG) << " failed" << std::endl;
                // }
            }

            // LOG(DEBUG) << std::endl;

        }

        /**
         * Answers an aggregate query. After calling this method, aggs should
         * contain the aggregate of all the points in region r. 
         */
        void aggregateOverRegion(A* aggs, RType* r) {
            aggs->reset();
            typename std::vector<U*>::const_iterator a;

            for(a = agents->begin(); a != agents->end(); a++) {
                if(r->template containsPoint<U,K>(*a)) {
                    aggs->combineFromOne(*a);
                }
            }
        }

        /**
         * Answers a knn query. 
         */
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
         * Updates the aggregate (data) value of a point. This method will be
         * called after the aggregate has been modified in the master copy of
         * the points. Updates to key attributes should be made by deleting
         * the point and then reinserting it. 
         */
        void afterUpdateAgg(U* u) {
            // Empty because we don't copy the units. 
        }

        /**
         * Inserts a point into the index. This method will be called after u
         * has been inserted into the master copy of the points. 
         */
        void afterInsert(U* u) {
            agents->push_back(u);
        }

        /**
         * Deletes a point from the index. This method will be called before u
         * has been deleted from the master copy of the points. 
         */
        void beforeDelete(U* u) {
            typename std::vector<U*>::iterator it;
            for(it = agents->begin(); it != agents->end(); it++) {
                if((*it)->id == u->id) {
                    agents->erase(it);
                    break;
                }
            }
        }

        /**
         * Updates the velocity of the specified point. This method will be
         * called before the velocity has been updated in the master copy. The
         * new position and velocity are speciifed . 
         */
        void beforeUpdateVelocity(U* point, 
                                  typename K::KeyType0 new0, 
                                  typename K::KeyType1 new1, 
                                  typename K::KeyType0 newV0, 
                                  typename K::KeyType0 newV1) {
            //Empty because we don't copy the units. 
        }


        /**
         * Performs any end-of-tick cleanup. If force is true, the index must
         * free all memory it holds. If updates are batched, they should be
         * performed here.
         */
        void endTick(bool force = false) {
            if(force) {
//                agents->clear();
                delete agents;
                agents = NULL;
            }
        }

        NaiveMovingIndex2D() {
            agents = NULL;
        }
        ~NaiveMovingIndex2D() {
            agents->clear();
            delete agents;
        }

    };
}


#endif /* _NAIVE_MOVING_INDEX_2D_H_ */

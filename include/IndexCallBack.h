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

#ifndef _INDEX_CALLBACK_H_
#define _INDEX_CALLBACK_H_

#include <iostream>
#include <vector>
#include <algorithm>

#include "JoinCallBack.h"

namespace PSimIndex {

    /**
     * This is a callback object that is passed to a SpatialIndex2D object to
     * during processing of enumeration or knn queries. The index should call
     * the reportPoint method for each point in the query answer. 
     *
     * Also provides an iterator interface to access the reported points.  
     *
     */
    template<typename T>
    class IndexCallBack {

    private: 
        std::vector<T*> agents;

    public:

        T* left;
        // JoinCallBack<T,T>* jc;
        // bool useJC;

        /**
         * The iterator type. 
         */
        typedef typename std::vector<T*>::iterator iterator;

        /**
         * Stores a point in this callback object. This should be called by an
         * index structure to report the answer to an enumeration or knn
         * query.
         */
        inline void reportPoint(T* agent) {

            //TODO: Don't use id here.
            // if(useJC && agent->id != left->id) {
            //     jc->reportPair(left, agent);
            // }
            // else {
                agents.push_back(agent);
            // }
        }

        /**
         * Returns the an iterator refering to the first point in this
         * callback object. 
         */
        inline iterator begin() {
            return agents.begin();
        }

        /**
         * Returns an iterator referering to the "past-the-end" point in this
         * callback object. 
         */
        inline iterator end() {
            return agents.end();
        }

        /**
         * Returns the number of points in this callback object. 
         */
        inline size_t size() {
            return agents.size();
        }

        /**
         * Clears this callback object. This does not delete any of the
         * reported points. 
         */
        inline void clear() {
            agents.clear();
        }


        /**
         * Sorts the points using the given comparator. See IndexUtils2D.h for
         * several standard comparators. 
         */
        template<class Compare>
        inline void sort(Compare comp) {
            std::sort(begin(), end(), comp);
        }


        /**
         * Testing method. Assumes objects of type T can be printed using <<
         * (This is true of the types in TestTypes2D.h). 
         */
        void print() {
            iterator it;

            for(it = agents.begin(); it != agents.end(); it++) {
                if((*it) == 0) {
                    std::cout << "NULL" << std::endl;
                }
                else {
                    std::cout << (**it) << std::endl;
                }
            }
        }

        IndexCallBack() {
        }

        // IndexCallBack(JoinCallBack<K,T,T>* jc) : jc(jc) {
        //     useJC = true;
        // }

    };
}

#endif /* _INDEX_CALLBACK_H_ */

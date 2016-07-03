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

#ifndef _VECTOR_GROUP_CALLBACK_H_
#define _VECTOR_GROUP_CALLBACK_H_

#include <vector>
#include <set>
//#include <tr1/unordered_map>
//#include <tr1/unordered_set>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <utility>
#include <algorithm>

#include "IndexLog.h"
#include "2D/IndexUtils2D.h"

namespace PSimIndex {

    /**
     * This is a callback object that is passed to a SpatialJoin2D object
     * during query processing. Since a join outputs pairs of points, this
     * callback also stores a vector of pairs and provides an iterator
     * interface and several convenience methods. 
     */
    template<typename K, typename T, typename S>
    class VectorGroupCallBack : public JoinCallBack<K,T,S> {

    private:
/*        typedef typename std::tr1::unordered_map<T*,
                                                 std::vector<S*>*, 
                                                 IDHash<K,T>, 
                                                 IDEq<K,T> > MapType;        

        typedef typename std::tr1::unordered_set<T*,
                                                 IDHash<K,T>,
                                                 IDEq<K,T> > KeySetType;
*/
    	 typedef typename std::unordered_map<T*,
    	                                                 std::vector<S*>*,
    	                                                 IDHash<K,T>,
    	                                                 IDEq<K,T> > MapType;

    	 typedef typename std::unordered_set<T*,
    	                                                 IDHash<K,T>,
    	                                                 IDEq<K,T> > KeySetType;

        MapType results;        
        KeySetType keySet;        
        bool copyPoints;
        size_t pairsCount;

    public:
        typedef typename KeySetType::iterator key_iterator;
        typedef typename std::vector<S*>::iterator val_iterator;

        
    public:

        bool getCopyPoints() {
            return copyPoints;
        }

        void setCopyPoints(bool copyPoints) {
            this->copyPoints = copyPoints;
        }

        /**
         * Stores a pair in this callback object. Should be called by the join
         * when reporting a query answer. 
         *
         * Memory model: when copyPoints is true, there is one copy of each T,
         * and potentially many copies of each S. When deleting, it is always
         * safe to delete the S entry, and once the set for a given T is
         * empty, we can delete the T.
         */
        inline void reportPair(T* a, S* b) {
            T* first;
            S* second;

            //std::cout << "Reporting pair: " << *a << ", " << *b << " " << copyPoints << std::endl;

            if(results.count(a) == 0) {
                if(copyPoints) {
                    first = new T(a);
                }
                else {
                    first = a;
                }

                //allocCount++;
                results[first] = new std::vector<S*>();

                keySet.insert(first);
            }

            if(copyPoints) {
                second = new S(b);
            }
            else {
                second = b;
            }


/*             for(int i = 0; i < results[a]->size(); i++) { */
/*                 if(K::getOID(results[a]->at(i)) == K::getOID(second)) { */
/*                     std::cerr << "Duplicate points! " << *a << " " << *second << std::endl; */
/*                 } */
/*             } */

            //results[a]->insert(second);
            results[a]->push_back(second);




            pairsCount++;
        }


//inline void deletePair(T* a, S* b) {
            // DBUTL_ASSERT((results.count(a) == 1 && results[a]->count(b) == 1));

            // //std::cout << "Deleting pair: " << *a << ", " << *b << std::endl;

            // T* key = *(keySet.find(a));
            // S* val = *(results[a]->find(b));
            
            // results[key]->erase(val);

            // if(copyPoints) {
            //     delete val;
            // }

            // if(results[key]->size() == 0) {
            //     deallocCount++;
            //     delete results[key];
            //     results.erase(key);
            //     keySet.erase(key);
            //     if(copyPoints) {
            //         delete key;
            //     }
            // }

            // pairsCount--;
        //}


        inline bool contains(T* key, S* value) {
            // return (results.count(key) > 0 && results[key]->count(value) > 0);
        }


        inline bool hasKey(T* key) {
            return (results.count(key) > 0);
        }

        /**
         * Returns the number of pairs in this callback object. 
         */
        // inline size_t size() {
        //     /return results.size();
        // }


        inline size_t numKeys() {
            return keySet.size();
        }
        
        inline size_t numMatches(T* key) {
            if(results.count(key) == 0) {
                return 0;
            }
            else {
                return results[key]->size();
            }
        }

        inline size_t numPairs() {
            return pairsCount;
        }




        inline key_iterator keyBegin() {
            return keySet.begin();
        }

        inline key_iterator keyEnd() {
            return keySet.end();
        }

        inline val_iterator matchBegin(T* key) {
            return results[key]->begin();
        }

        inline val_iterator matchEnd(T* key) {
            return results[key]->end();
        }

        // bool equals(JoinCallBack<K,T,S>* other) {
            //std::cout << "In equals" << std::endl;

            //TODO: Should I use log here??
            // bool res = true;


            // if(numKeys() != other->numKeys()) {
            //     LOG(ERR) << "Error: Join Results have a different number of keys: left = "
            //              << numKeys() << " right = " << other->numKeys() << std::endl;
            //     res = false;
            // } 

            // typename JoinCallBack<K,T,S>::key_iterator kit;
            // typename JoinCallBack<K,T,S>::val_iterator vit;

            // for(kit = other->keyBegin(); kit != other->keyEnd(); kit++) {
            //     T* key = *kit;

            //     if(!hasKey(key)) {
            //         LOG(ERR) << "Left is missing key: " << *key << std::endl;
            //         res = false;
            //     }
            //     else {
            //         for(vit = other->matchBegin(key); vit != other->matchEnd(key); vit++) {
            //             if(!contains(key, *vit)) {
            //                 LOG(ERR) << "Left is missing pair: " << *key
            //                          << " " << **vit << std::endl;
            //                 res = false;
            //             }
            //         }
            //     }
            // }

            // for(kit = keyBegin(); kit != keyEnd(); kit++) {
            //     T* key = *kit;

            //     if(!other->hasKey(key)) {
            //         LOG(ERR) << "Right is missing key: " << *key << std::endl;
            //         res = false;
            //     }
            //     else {
            //         for(vit = matchBegin(key); vit != matchEnd(key); vit++) {
            //             if(!other->contains(key, *vit)) {
            //                 LOG(ERR) << "Left is missing pair: " << *key
            //                          << " " << **vit << std::endl;
            //                 res = false;
            //             }
            //         }
            //     }
            // }

            // return res;
        //}


        /**
         * TODO: Leaks memory. Shouldn't need to pass in Compare object --
         * instead use K to order keys and values by id. 
         */
        void printSorted() {
            typename std::vector<T*>::iterator kit;
            val_iterator vit;

            std::vector<T*> sortedKeys(keyBegin(), keyEnd());


            std::sort(sortedKeys.begin(), sortedKeys.end(), idlt<T,K>);
             


            for(kit = sortedKeys.begin(); kit != sortedKeys.end(); kit++) {
                std::vector<S*>* res = results[*kit];

                //No reason we can't sort this vectors in place...
                std::sort(res->begin(), res->end(), idlt<S,K>);
                for(vit = res->begin(); vit != res->end(); vit++) {
                    std::cout << "(" << **kit << ", "
                              << **vit << ")" << std::endl;
                }
             }

             // std::vector<std::pair<T*,S*>*> vec;

            // for(it = keyBegin(); it != keyEnd(); it++) {

            //     T* key = it->first;

            //     typename JoinCallBack<K,T,S>::val_iterator valIt;

            //     for(valIt = matchBegin(key); valIt != matchEnd(key); valIt++) {
            //         vec.push_back(new std::pair<T*,S*>(key, *valIt));
            //     }
            // }

            // std::sort(vec.begin(), vec.end(), comp);

            // typename std::vector<std::pair<T*,S*>*>::iterator vIt;
            
            // for(vIt = vec.begin(); vIt != vec.end(); vIt++) {
            //     if((*vIt) == 0)
            //         std::cout << "NULL" << std::endl;
            //     else {
            //         std::cout << "(" << *((*vIt)->first) << ", "
            //                   << *((*vIt)->second) << ")" << std::endl;
            //     }
            // }
        }


        /**
         * Clears this callback object. This does not delete any of the
         * underlying points unless copyPoints is true. 
         */
        inline void clear() {
            typename MapType::iterator it;
            val_iterator vit;

            for(it = results.begin(); it != results.end(); it++) {
                if(copyPoints) {
                    for(vit = it->second->begin(); vit != it->second->end(); vit++) {
                        delete *vit;
                    }
                    delete it->first;
                }
                it->second->clear();
                //deallocCount++;
                delete it->second;
            }

	  results.clear();
          keySet.clear();

	  pairsCount = (size_t) 0;
          //allocCount = deallocCount = 0;
        }

        VectorGroupCallBack(bool copyPoints = false) : copyPoints(copyPoints) {
            pairsCount = (size_t) 0;
        }
        
       ~VectorGroupCallBack() {
           clear();
       }

    };

}

#endif /* _VECTOR_GROUP_CALLBACK_H_ */

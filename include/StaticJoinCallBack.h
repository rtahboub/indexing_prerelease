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

#ifndef _STATIC_JOIN_CALLBACK_H_
#define _STATIC_JOIN_CALLBACK_H_

#include <vector>
#include <set>
#include <list>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include <iostream>
#include <utility>
#include <algorithm>

#include "IndexLog.h"
#include "SimpleHash.h"
namespace PSimIndex {

    /**
     * This is a callback object that is passed to a SpatialJoin2D object
     * during query processing. Since a join outputs pairs of points, this
     * callback also stores a vector of pairs and provides an iterator
     * interface and several convenience methods. 
     */
    template<typename K, typename T, typename S>
    class StaticJoinCallBack : public JoinCallBack<K,T,S> {

    private:


        typedef typename std::tr1::unordered_map<T*, SimpleHash*, IDHash<K,T>, IDEq<K,T> > MapType;
        typedef SimpleHash KeySetType;

        //typedef typename std::tr1::unordered_set<S*,IDHash<K,S>,IDEq<K,S> > ValSetType;
        //typedef typename google::dense_hash_set<S*,IDHash<K,S>,IDEq<K,S> > ValSetType;


        MapType results;
        KeySetType keySet;

        bool copyPoints;
        size_t pairsCount;

        int allocCount;
        int deallocCount;


        class SimpleHashPool {
        private:
            std::list<SimpleHash*> lst;
            
        public:
            SimpleHash* alloc() {
                if(lst.size() == 0) {
                    return new SimpleHash();
                }
                else {
                    SimpleHash* ret = lst.front();
                    lst.pop_front();
                    return ret;
                }
            }
            void dealloc(SimpleHash* hash) {
                lst.push_back(hash);
            }

            ~SimpleHashPool() {
                while(lst.size() != 0) {
                    SimpleHash* s = lst.front();
                    lst.pop_front();
                    delete s;
                }
            }

        };

        SimpleHashPool sp;

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
                //std::cout << "HERE" << std::endl;

                // if(copyPoints) {
                //     first = new T(a);
                // }
                // else {
                    first = a;
                // }

                allocCount++;
                results[first] = sp.alloc();
                //results[first] = new SimpleHash();

                keySet.insert(first);
            }

            // if(copyPoints) {
            //     second = new S(b);
            // }
            else {
                second = b;
            }

            //std::cout << "inserting " << *second << "into first: " << *a << std::endl;
            results[a]->insert(second);
            pairsCount++;
        }


        inline void deletePair(T* a, S* b) {
            DBUTL_ASSERT((results.count(a) == 1 && results[a]->count(b) == 1));

            // //std::cout << "Deleting pair: " << *a << ", " << *b << std::endl;

            // T* key = *(keySet.find(a));
            // S* val = *(results[a]->find(b));
            
            // results[key]->erase(val);

            // if(copyPoints) {
            //     delete val;
            // }

            // if(results[key]->size() == 0) {
            //     deallocCount++;
            //     //delete results[key];
            //     //sp.free(results[key]);

            //     results.erase(key);
            //     keySet.erase(key);
            //     if(copyPoints) {
            //         delete key;
            //     }
            // }

            // pairsCount--;
        }


        inline bool contains(T* key, S* value) {
            return (results.count(key) > 0 && results[key]->contains(value) > 0);
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

        // typedef typename JoinCallBack<K,T,S>::template SetIterImpl<T*> KeySetIterImpl;
        // typedef typename JoinCallBack<K,T,S>::template SetIterImpl<S*> ValSetIterImpl;


        // inline typename JoinCallBack<K,T,S>::key_iterator keyBegin() {
        //     //return typename JoinCallBack<K,T,S>::key_iterator(new KeySetIterImpl(keySet.begin()));

        //     return keySet.begin();
        // }

        // inline typename JoinCallBack<K,T,S>::key_iterator keyEnd() {
        //     //return typename JoinCallBack<K,T,S>::key_iterator(new KeySetIterImpl(keySet.end()));
        //     return keySet.end();
        // }

        // inline typename JoinCallBack<K,T,S>::val_iterator matchBegin(T* key) {
        //     // TODO: change to exception ? 
        //     DBUTL_ASSERT((results.count(key) > 0)); 
            
        //     //return typename JoinCallBack<K,T,S>::key_iterator(new ValSetIterImpl(results[key]->begin()));
            
        //     //return results[key]->begin();
        // }

        // inline typename JoinCallBack<K,T,S>::val_iterator matchEnd(T* key) {
        //     // TODO: change to exception ? 
        //     DBUTL_ASSERT((results.count(key) > 0)); 
        //     //return results[key]->end();
        //     //return typename JoinCallBack<K,T,S>::key_iterator(new ValSetIterImpl(results[key]->end()));
        // }


        bool equals(JoinCallBack<K,T,S>* other) {
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
        }


        /**
         * TODO: Leaks memory. Shouldn't need to pass in Compare object --
         * instead use K to order keys and values by id. 
         */
        void printSorted() {

            // //std::cout << "In printSorted" << std::endl;

            // typename JoinCallBack<K,T,S>::key_iterator it;
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
            //std::cout << "In clear: " << std::endl;
            
            //typename KeySetType::iterator kit;

            // if(copyPoints) {
            //     for(kit = keySet.begin(); kit != keySet.end(); kit++) {
            //         delete *kit;
            //     }
            //}


            typename MapType::iterator it;
            //typename JoinCallBack<K,T,S>::val_iterator vit;                
            //typename ValSetType::iterator vit;



            for(it = results.begin(); it != results.end(); it++) {
                // if(copyPoints) {
                //     for(vit = it->second->begin(); vit != it->second->end(); vit++) {
                //         delete *vit;
                //     }
                //     delete it->first;
                // }
                it->second->clear();
                deallocCount++;
                //delete it->second;
                sp.dealloc(it->second);
                //sp.free(it->second);
            }

            results.clear();
            keySet.clear();

	  // pairsCount = (size_t) 0;
          
          // //std::cout << "allocCount: " << allocCount << " deallocCount: " << deallocCount << std::endl;
          // allocCount = deallocCount = 0;

          //std::cout << "After clear..." << std::endl;

        }

        StaticJoinCallBack(bool copyPoints = false) : copyPoints(copyPoints) {


            // std::cout << "in StaticJoinCallBack(): copyPoints "  << copyPoints << std::endl;

            allocCount = 0;
            deallocCount = 0;

            pairsCount = (size_t) 0;
        }
        
       ~StaticJoinCallBack() {


           // std::cout << "In destructor" << std::endl;
           clear();
       }

    };

}



#endif /* _STATIC_JOIN_CALLBACK_H_ */

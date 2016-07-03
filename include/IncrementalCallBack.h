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


#ifndef _INCREMENTAL_CALLBACK_H_
#define _INCREMENTAL_CALLBACK_H_


//#include <tr1/unordered_set>
//#include <tr1/unordered_map>

#include <unordered_set>
#include <unordered_map>

#include <list>

#include "JoinCallBack.h"
//#include "StaticJoinCallBack.h"
#include "IndexLog.h"
//#include "2D/IndexUtils2D.h"

namespace PSimIndex {

    template<typename K, typename T, typename S>
    class IncrementalCallBack : public JoinCallBack<K,T,S> {
    private:

//        typedef typename std::tr1::unordered_set<T*, IDHash<K,T>, IDEq<K,T> > LeftSetType;
//        typedef typename std::tr1::unordered_set<S*, IDHash<K,S>, IDEq<K,S> > RightSetType;

//        typedef typename std::tr1::unordered_map<T*,RightSetType*, IDHash<K,T>, IDEq<K,T> > LeftMapType;
//        typedef typename std::tr1::unordered_map<S*,LeftSetType*, IDHash<K,S>, IDEq<K,S> > RightMapType;


        typedef typename std::unordered_set<T*, IDHash<K,T>, IDEq<K,T> > LeftSetType;
         typedef typename std::unordered_set<S*, IDHash<K,S>, IDEq<K,S> > RightSetType;

         typedef typename std::unordered_map<T*,RightSetType*, IDHash<K,T>, IDEq<K,T> > LeftMapType;
         typedef typename std::unordered_map<S*,LeftSetType*, IDHash<K,S>, IDEq<K,S> > RightMapType;



        LeftMapType queryMap;
        RightMapType pointMap;
        LeftSetType keySet;


        bool copyPoints;
        size_t pairCount;

    public:


        typedef typename LeftSetType::iterator key_iterator;
        typedef typename RightSetType::iterator val_iterator;

        inline bool getCopyPoints() {
            return copyPoints;
        }

        inline void setCopyPoints(bool copyPoints) {
            this->copyPoints = copyPoints;
        }

        //inline void addResult(T* a, S* b) {
        void reportPair(T* a, S* b) {
            T* first;
            S* second;
            
            //std::cout << "Adding result " << *a << " " << *b << std::endl;

            if(queryMap.count(a) == 0) {
                if(copyPoints) {
                    first = new T(a);
                }
                else {
                    first = a;
                }
                queryMap[first] = new RightSetType();
                keySet.insert(first);
            }
            else {
                first = *keySet.find(a);
            }

            if(pointMap.count(b) == 0) {
                if(copyPoints) {
                    second = new S(b);
                }
                else {
                    second = b;
                }
                pointMap[second] = new LeftSetType();
            }
            else {
                second = pointMap.find(b)->first;
            }

            // if(queryMap[first]->count(second) == 1) {
            //     std::cerr << "Already contains 1"<< std::endl;
            // }

            // if(pointMap[second]->count(first) == 1) {
            //     std::cerr << "Already contains 2"<< std::endl;
            // }

            if(queryMap[first]->count(second) == 1) {
                DBUTL_ASSERT((pointMap[second]->count(first) == 1));
                LOG(ERR) << "Inserting duplicate result" << std::endl;
            }
            else {
                pairCount++;
            }


            queryMap[first]->insert(second);
            pointMap[second]->insert(first);
            


        }

        void deletePair(T* a, S* b, int tickNum = 0) {
            // if(tickNum == 8) {
            //     std::cout << *a << " " << *b << " " << std::endl;
            // }


            DBUTL_ASSERT((queryMap.count(a) == 1));
            DBUTL_ASSERT((queryMap[a]->count(b) == 1));

            DBUTL_ASSERT((pointMap.count(b) == 1 && pointMap[b]->count(a) == 1));

            // We use find so that we get the correct pointer when copyPoints is true.
            T* query = queryMap.find(a)->first;
            S* point = pointMap.find(b)->first;


            queryMap[a]->erase(b);
            pointMap[b]->erase(a);
            
            if(queryMap[a]->size() == 0) {

                delete queryMap[a];
                queryMap.erase(a);
                keySet.erase(a);

                if(copyPoints) {
                    delete query;
                }
            }

            if(pointMap[b]->size() == 0) {
                delete pointMap[b];
                pointMap.erase(b);

                if(copyPoints) {
                    delete point;
                }
            }

            pairCount--;
        }


        inline void deleteRight(S* b) {
            while(pointMap.count(b) != 0) {
                deletePair(*pointMap[b]->begin(), b);
                pairCount--;
            }
        }


        inline void deleteLeft(T* a) {
            while(numMatches(a) > 0) {
                deletePair(a, *matchBegin(a));
                pairCount--;
            }
        }




        inline bool contains(T* a, S* b) {
            return (queryMap.count(a) > 0 && queryMap[a]->count(b) > 0);
        }


        inline bool hasKey(T* a) {
            return (keySet.count(a) > 0);
        }

        inline size_t numKeys() {
            return keySet.size();
        }

        inline size_t numMatches(T* key) {
            if(queryMap.count(key) == 0) {
                return 0;
            }
            else {
                return queryMap[key]->size();
            }
        }

        inline size_t numPairs() {
            return pairCount;
        }




        // typedef typename JoinCallBack<K,T,S>::template SetIterImpl<T*> KeySetIterImpl;
        // typedef typename JoinCallBack<K,T,S>::template SetIterImpl<S*> ValSetIterImpl;

        inline key_iterator keyBegin() {
            //SetIterImpl<T*>* impl = new SetIterImpl<T*>(keySet.begin())
            //return JoinCallBack<K,T,S>::key_iterator(new KeySetIterImpl(keySet.begin()));
            return keySet.begin();
        }

        inline key_iterator keyEnd() {
            //return JoinCallBack<K,T,S>::key_iterator(new KeySetIterImpl(keySet.end()));
            return keySet.end();
        }

        inline val_iterator matchBegin(T* key) {
            // TODO: change to exception ? 
            DBUTL_ASSERT((queryMap.count(key) > 0)); 
            return queryMap[key]->begin();

            //return JoinCallBack<K,T,S>::key_iterator(new ValSetIterImpl(queryMap[key]->begin()));
        }

        inline val_iterator matchEnd(T* key) {
            // TODO: change to exception ? 
            DBUTL_ASSERT((queryMap.count(key) > 0)); 
            return queryMap[key]->end();
            //return JoinCallBack<K,T,S>::key_iterator(new ValSetIterImpl(queryMap[key]->end()));
        }


        // TODO: Merge with StaticJoinCallBack implementation.
        bool equals(IncrementalCallBack<K,T,S>* other) {
            // //TODO: Should I use log here??
            bool res = true;

            if(numKeys() != other->numKeys()) {
                LOG(ERR) << "Error: Join Results have a different number of keys: left = "
                         << numKeys() << " right = " << other->numKeys() << std::endl;
                res = false;
            } 



            key_iterator kit;
            val_iterator vit;

            for(kit = other->keyBegin(); kit != other->keyEnd(); kit++) {
                T* key = *kit;

                if(!hasKey(key)) {
                    LOG(ERR) << "Left is missing key: " << *key << std::endl;
                    res = false;
                }
                else {
                    for(vit = other->matchBegin(key); vit != other->matchEnd(key); vit++) {
                        if(!contains(key, *vit)) {
                            LOG(ERR) << "Left is missing pair: " << *key
                                     << " " << **vit << std::endl;
                            res = false;
                        }
                    }
                }
            }

            for(kit = keyBegin(); kit != keyEnd(); kit++) {
                T* key = *kit;

                if(!other->hasKey(key)) {
                    LOG(ERR) << "Right is missing key: " << *key << std::endl;
                    res = false;
                }
                else {
                    for(vit = matchBegin(key); vit != matchEnd(key); vit++) {
                        if(!other->contains(key, *vit)) {
                            LOG(ERR) << "Left is missing pair: " << *key
                                     << " " << **vit << std::endl;
                            res = false;
                        }
                    }
                }
            }



            if(numPairs() != other->numPairs()) {

                for(kit = other->keyBegin(); kit != other->keyEnd(); kit++) {
                    T* key = *kit;

                    if(numMatches(key) != other->numMatches(key)) {
                        std::cout << "key: " << *key << std::endl;
                        std::cout << "numMatches: " << numMatches(key) << " " 
                                  << other->numMatches(key) << std::endl;
                        
                        std::cout << "Left matches..." << std::endl;
                        for(vit = matchBegin(key); vit != matchEnd(key); vit++) {
                            std::cout << **vit << std::endl;
                        }
                        std::cout << std::endl;

                        std::cout << "Right matches..." << std::endl;
                        for(vit = other->matchBegin(key); vit != other->matchEnd(key); vit++) {
                            std::cout << **vit << std::endl;
                        }


                    }

                }

                std::cout << std::endl;


            }



            return res;
        }

        /**
         * Less than comparator that uses the oid of the point. This can be passed
         * to std::sort to sort an stl collection.
         *
         * TODO: This is in indexUtils2D, but caused a circular dependency. Think about 
         * where to put it.
         */
        template<typename U, typename K2>
        static bool idlt(U* a, U* b) {
            return (K2::getOID(a) < K2::getOID(b));
        }

        void printSorted() {

            typename std::vector<T*>::iterator kit;

            std::vector<T*> sortedKeys(keyBegin(), keyEnd());
            std::sort(sortedKeys.begin(), sortedKeys.end(), idlt<T,K>);

            for(kit = sortedKeys.begin(); kit != sortedKeys.end(); kit++) {
                //RightSetType* val = queryMap[*kit];
                std::vector<S*> res(matchBegin(*kit), matchEnd(*kit));
                std::sort(res.begin(), res.end(), idlt<S,K>);


                typename std::vector<S*>::iterator vit;
                for(vit = res.begin(); vit != res.end(); vit++) {
                    std::cout << "(" << **kit << ", "
                              << **vit << ")" << std::endl;
                }
            }

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

        inline void clear() {

            //std::cout << "In Incremental CallBack clear" << std::endl;
            key_iterator kit;
            val_iterator vit;

            typename LeftMapType::iterator lit;
            typename RightMapType::iterator rit;

            // Use the fact that every point appears that appears in one of
            // the sets appears as a key in one of the maps. 
            for(lit = queryMap.begin(); lit != queryMap.end(); lit++) {
                delete lit->second;

                if(copyPoints) {
                    delete lit->first;
                }
            }

            for(rit = pointMap.begin(); rit != pointMap.end(); rit++) {
                delete rit->second;

                if(copyPoints) {
                    delete rit->first;
                }
            }

            queryMap.clear();
            pointMap.clear();
            keySet.clear();
            pairCount = (size_t) 0;

        }


        IncrementalCallBack(bool copyPoints = false) : copyPoints(copyPoints) {
            pairCount = (size_t) 0;
        }
        ~IncrementalCallBack() {
            //std::cout << "In Incremental destructor" << std::endl;
            clear();
        }
    };
}


#endif /* _INCREMENTAL_CALLBACK_H_ */

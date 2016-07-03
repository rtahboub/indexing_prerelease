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

#ifndef _JOIN_CALLBACK_H_
#define _JOIN_CALLBACK_H_

#include <vector>
//#include <tr1/unordered_set>
//#include <tr1/unordered_map>

#include <unordered_set>
#include <unordered_map>

#include <iostream>
#include <utility>
#include <algorithm>

namespace PSimIndex {


    /**
     * TODO: Ben: This class has been changing a lot and is currently a bit of
     * a mess. The plan is to have include ONLY insert operations to avoid
     * problems with iterators. Some thought is needed to decide whether
     * some/all of the incremental methods should be included.  
     */





    //TODO: Haven't I already overloaded these in TestTypes2D??
    template<typename K, typename U>
    struct IDHash {
    private:
       // std::tr1::hash<typename K::IDType> intHsh;
        std::hash<typename K::IDType> intHsh;

    public:
        std::size_t operator()(U* p) const {
            //std::cout << "calling hash on: " << *p << " " << intHsh(K::getOID(p)) << std::endl;
            //return intHsh(K::getOID(p));
            return (size_t)(K::getOID(p));
        }
    };
    
    template<typename K, typename U>
    struct IDEq {
        
    public:
        bool operator()(const U* const p1, const U* const p2) const {
            // std::cout << "Calling equal on: " << *p1 << " " << *p2 << std::endl;
            return (K::getOID(p1) == K::getOID(p2));
        }
    };

        
    /**
     * The iterators make assumptions about the implementation of inherited
     * classes. Need to make this more generic. 
     */
    template<typename K, typename T, typename S>
    class JoinCallBack {

    public:

        //TODO: I should really figure out how to do this at somepoint...
        //I was trying to do something like
        // http://stackoverflow.com/questions/2989200/c-iterators-and-inheritance


        //
        // template<typename DataType>
        // class IterImpl {
        // public:
        //     virtual void getType()
        //     virtual void incr() = 0;
        //     virtual DataType& deref() = 0;
        //     //virtual IterImpl<DataType>* clone() = 0;
        //     virtual bool equals(IterImpl<DataType>* other) = 0;
        // };



        
        // template<typename DataType>
        // class VectorIterImpl : public IterImpl<DataType> {
        // private:
        //     typename std::vector<DataType>::iterator it;
        // public:


        //     void incr() {
        //         it++;
        //     }

        //     DataType& deref() {
        //         return *it;
        //     }

        //     IterImpl<DataType>* clone() {
        //         return new VectorIterImpl<DataType>(it);
        //     }

        //     bool equals(IterImpl<DataType>* other) {
        //         other->
        //     }

        //     VectorIterImpl(typename std::vector<DataType>::iterator it) {
        //         this->it = it;
        //     }
        // };


        // template<typename DataType>
        // class SetIterImpl : public IterImpl<DataType> {
        // private:
        //     typename std::tr1::unordered_set<DataType,
        //                                      IDHash<K,DataType>, 
        //                                      IDEq<K,DataType> >::iterator it;
        // public:
        //     void incr() {
        //         it++;
        //     }

        //     DataType& deref() {
        //         return *it;
        //     }

        //     IterImpl<DataType>* clone() {
        //         return new SetIterImpl<DataType>(it);
        //     }

        //     SetIterImpl(typename std::tr1::unordered_set<DataType,
        //                                                  IDHash<K,DataType>, 
        //                                                  IDEq<K,DataType> >::iterator it) {
        //         this->it = it;
        //     }
        // };


    private:

        // template<typename DataType>
        // class iterator {
        // private:
        //     IterImpl<DataType>* impl;

        // public:
        //     iterator<DataType>& operator=(iterator<DataType> const& right) {
        //         delete impl;
        //         impl = right.impl->clone();
        //         return *this;
        //     }

        //     bool operator==(iterator<DataType>& rhs) const {
        //         return impl->
        //     }
            


        //     iterator<DataType>& operator++() {
        //         impl->incr();
        //         return *this;
        //     }

        //     iterator<DataType> operator++(int) {
        //         iterator<DataType> copy(impl->clone());
        //         impl->incr();
        //         return copy;
        //     }

        //     DataType& operator*() {
        //         return impl->deref();
        //     }

        //     iterator() {
        //         this->impl = NULL;
        //     }

        //     iterator(IterImpl<DataType>* impl) {
        //         this->impl = impl;
        //     }

        //     ~iterator() {
        //         delete impl;
        //     }
        // };

    public:

        // typedef iterator<T*> key_iterator;
        // typedef iterator<S*> val_iterator;


        // TODO: This could be done better. See
        // http://stackoverflow.com/questions/2989200/c-iterators-and-inheritance

        //typedef typename std::tr1::unordered_set<S*,IDHash<K,S>,IDEq<K,S> > SetType;

        // typedef typename std::tr1::unordered_set<T*,IDHash<K,T>,IDEq<K,T> >::iterator key_iterator;
        // typedef typename SetType::iterator val_iterator;
        
        virtual bool getCopyPoints() = 0;
        virtual void setCopyPoints(bool copyPoints) = 0;

        virtual void reportPair(T* a, S* b) = 0;
        //virtual void deletePair(T* a, S* b) = 0;

        //virtual bool contains(T* a, S* b) = 0;
        //virtual bool hasKey(T* a) = 0;

        //virtual size_t size() = 0;
        // virtual size_t numKeys() = 0;
        // virtual size_t numMatches(T* key) = 0;  
        virtual size_t numPairs() = 0;

        // virtual key_iterator keyBegin() = 0;
        // virtual key_iterator keyEnd() = 0;

        // virtual val_iterator matchBegin(T* key) = 0;
        // virtual val_iterator matchEnd(T* key) = 0;
        
        // virtual bool equals(JoinCallBack<K,T,S>* other) = 0;


         virtual void printSorted() = 0;


        virtual void clear() = 0;

        virtual ~JoinCallBack() {}

    };


}



#endif /* _JOIN_CALLBACK_H_ */

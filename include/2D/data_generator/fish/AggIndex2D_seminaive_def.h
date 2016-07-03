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

/*
 *  AggIndex2D_seminaive_def.h
 *  TemplateTests
 *
 *  Created by Alan Demers on 5/21/09.
 *  Copyright 2009 Cornell. All rights reserved.
 *
 */

#ifndef _AGG_INDEX_SEMINAIVE_DEF_H_
#define _AGG_INDEX_SEMINAIVE_DEF_H_

/*
 * Definition of seminaive 2D aggregate index template.
 */

#include <algorithm>

#include "2D/data_generator/fish/AggIndex2D_seminaive_dcl.h"
#include "2D/data_generator/fish/FishRegion.h"


template<typename U, typename K, typename A>
class AggIndex2D_seminaive<U, K, A>::Rep {
public:
    static inline bool k0lt (const U *a, const U *b) { return K::getK0(a) < K::getK0(b); }
    // static inline bool k1lt (const U *a, const U *b) { return K::getK1(a) < K::getK1(b); }
    std::vector<U*> * uvp0;
    // std::vector<U*> * uvp1;
    Rep() : uvp0(0) {}
    ~Rep() { if( uvp0 != 0 ) delete uvp0; }
};

template<typename U, typename K, typename A>
void AggIndex2D_seminaive<U, K, A>::startEpoch(std::vector<U*> *uvp) {
    std::vector<U*> *p0 = rep->uvp0;
    if( (p0 == 0) || (p0->size() != uvp->size()) ) {
        delete p0; // okay if 0
        p0 = new std::vector<U*>(*uvp);
    } else {
        // TODO: memcpy?
        for( int i = 0; i < uvp->size(); i++ ) {
            (*p0)[i] = (*uvp)[i];
        }
    }
    std::sort( p0->begin(), p0->end(), Rep::k0lt );
    rep->uvp0 = p0;
}

template<typename U, typename K, typename A>
template<typename R>
void AggIndex2D_seminaive<U, K, A>::aggregateOverRegion(A *aggs, R *r) {
#if 1
    ORange2D orange;  boundingRegion(orange, *r);
    std::vector<U*> * p0 = rep->uvp0;
    int i = 0;  int j = p0->size();
    while( i < (j-1) ) {
        int m = (i + j) / 2;
        if( K::getK0(p0->operator[](m)) < orange.x0 ) { i = m; } else { j = m; }
    }
    for(;;) {
        if( i >= p0->size() ) goto Out;
        U * up = p0->operator[](i);
        if( K::getK0(up) >= orange.x1 ) goto Out;
        if( r->contains(K::getK0(up), K::getK1(up)) ) {
            aggs->combineFromOne(up);
        }
        i++;
    }
Out: ;
#endif
    return;
}

#if 1
template<typename U, typename K, typename A>
template<typename CB, typename R>
int AggIndex2D_seminaive<U, K, A>::enumerateOverRegion(CB *cb, R *r) {
    int ans = 0;
    ORange2D orange;  boundingRegion(orange, *r);
    std::vector<U*> * p0 = rep->uvp0;
    int i = 0;  int j = p0->size();
    while( i < (j-1) ) {
        int m = (i + j) / 2;
        if( K::getK0(p0->operator[](m)) < orange.x0 ) { i = m; } else { j = m; }
    }
    for(;;) {
        if( i >= p0->size() ) goto Out;
        U * up = p0->operator[](i);
        if( K::getK0(up) >= orange.x1 ) goto Out;
        if( r->contains(K::getK0(up), K::getK1(up)) ) {
            ans = (*cb)(up);  if( ans < 0 ) goto Out;
        }
        i++;
    }
Out: ;
    return ans;
}
#else
template<typename U, typename K, typename A>
template<typename CB, typename R>
int AggIndex2D_seminaive<U, K, A>::enumerateOverRegion(CB *cb, R *r) {
    int ans = 0;
    ORange2D orange;  boundingRegion(orange, *r);
    std::vector<U*> * p0 = rep->uvp0;
    U hack;
    hack.setX(orange.x0);
    typename std::vector<U*>::iterator istart =  std::lower_bound( p0->begin(), p0->end(), &hack, Rep::k0lt );
    hack.setX(orange.x1);
    typename std::vector<U*>::iterator ilim = std::lower_bound( istart, p0->end(), &hack, Rep::k0lt );
    for( ; istart != ilim; istart++ ) {
        U *up = *istart;
        if( r->contains(K::getK0(up), K::getK1(up)) ) {
            ans = (*cb)(up);  if( ans < 0 ) break;
        }
    }
    return ans;
}
#endif

template<typename U, typename K, typename A>
template<typename CB, typename R>
int AggIndex2D_seminaive<U, K, A>::nearestK(CB *cb, int k, U *u, R *r) {
    return 0;
}

template<typename U, typename K, typename A>
void AggIndex2D_seminaive<U, K, A>::finishEpoch(bool force) {
    if( force ) {
        delete rep->uvp0; // okay if 0
        rep->uvp0 = 0;
    }
}

template<typename U, typename K, typename A>
AggIndex2D_seminaive<U, K, A>::AggIndex2D_seminaive() {
    rep = new Rep();
}

template<typename U, typename K, typename A>
AggIndex2D_seminaive<U, K, A>::~AggIndex2D_seminaive() {
    delete rep;
}


#endif /* _AGG_INDEX_SEMINAIVE_DEF_H_ */

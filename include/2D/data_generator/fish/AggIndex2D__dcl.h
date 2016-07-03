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
 *  AggIndex2D__dcl.h
 *  TemplateTests
 *
 *  Created by Alan Demers on 5/14/09.
 *  Copyright 2009 Cornell. All rights reserved.
 *
 */

/*
 * TODO: it might be a good idea to template-ize wrt the return type of getK0 and getK1 (e.g. int or float)?
 */

/*
 * DO NOT INCLUDE THIS HEADER FILE DIRECTLY
 *
 * Include it through one of its specializations, e.g.
 *
 *      #include "AggIndex2D_naive.h"
 *
 * The preprocessor variable AGG_INDEX_2D must be set to the specialized name of the
 * index class -- e.g. AggIndex2D_naive -- by the evironment including this file
 */

#include <vector>

/*
 * Generic declaration of 2-dimensional aggregate index.
 *
 *      template <typename U, typename K, typename A> class AggIndex2D_xxxxxx { ... }
 *
 * - xxxxxx = a unique "kind" associated with the index algorithm.
 *
 * - U is the "Unit" class.  There are no specific requirements.
 *
 * - K is the "Keys" class.
 *   It must have static (preferably inline) methods
 *      double getK0(U*) and double getK1(U*)
 * These are the keys on which the index is built.
 *
 * - A is the "AggregateValues" class.
 *   This has all the aggregates that are to be computed.
 *   The index implementation needs
 *      [inline] void reset(); // set to idempotent value
 *      [inline] void combine(A *); // the o-plus operation
 *      [inline] void combineFromOne(U *); // o-plus from a single Unit
 *
 */

#ifndef AGG_INDEX_2D
#   error naked include of AggIndex2D__dcl.h
#endif

template <typename U, typename K, typename A>
class AGG_INDEX_2D {
    
private:
    class Rep;
    Rep * rep;
    
public:
    /*
     * create index on vector of Unit pointers
     */
    void startEpoch(std::vector<U*> * uvp);
    
    /*
     * set *aggs to aggregate values of all Units in region r
     */
    template <typename R>
    void aggregateOverRegion(A *aggs, R *r);
    
    /*
     * apply callback cb to every Unit in region r
     * return last result returned by cb
     * terminate immediately if cb returns a negative result
     */
    template <typename CB, typename R>
    int enumerateOverRegion(CB *cb, R *r);
    
    /*
     * apply callback cb to each of the k Units nearest u in region r
     * the calls are done in closest-to-farthest order
     * return the last result returned by cb
     * terminate immediately if cb returns a negative result
     */
    template <typename CB, typename R>
    int nearestK(CB *cb, int k, U *u, R *r);
    
    /*
     * release resources associated with an index
     * (optional unless force is true)
     */
    void finishEpoch(bool force);
    
    AGG_INDEX_2D();
    ~AGG_INDEX_2D();
};

#undef AGG_INDEX_2D


#ifndef _AGG_INDEX_2D__DCL_H_
#define _AGG_INDEX_2D__DCL_H_

/*
 * Standard keys class for X-Y indexing
 */

template <typename U>
struct StdKeys2D {
    static inline double getK0(const U *u) { return u->getX(); }
    static inline double getK1(const U *u) { return u->getY(); }
};

#endif /* _AGG_INDEX_2D__DCL_H_ */

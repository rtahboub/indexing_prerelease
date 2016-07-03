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

#ifndef _INDEX_UTILS_H_
#define _INDEX_UTILS_H_

#include "util/Log.h"

#include "Region2D.h"
#include "2D/TestTypes2D.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cstdio>
#include <cmath>

using namespace DBUtl;

/**
 * Utility functions for 2D spatial indices. Please add any methods here that
 * may be useful for multiple indices. Note that function template that
 * include the type K will have to be explicitly instantiated since K is
 * static, i.e. I would write
 *   closer<U, K>(target, u1, u2);
 */
namespace PSimIndex {
 

    template<typename T>
    void printPtrVector(std::vector<T*>* vec) {
        typename std::vector<T*>::const_iterator it; 

        for(it = vec->begin(); it != vec->end(); it++) {
            std::cout << **it << std::endl;
        }
        std::cout << std::endl;
    }




    /**
     * Partitions a vector into 
     * 
     * The points in each partition remain ordered in the same way that they
     * were before.
     *
     */
    template<typename U, typename K>
    void partitionVectorStably(std::vector<U*>*  points, 
                               std::vector<U*>*  pointsLow, 
                               std::vector<U*>*  pointsHigh,
                               U* splitPoint, 
                               int dim) {
        DBUTL_ASSERT(dim == 0 || dim ==1);
        for(int i = 0; i < points->size(); i++) {
            if(dim == 0) {
                typename K::KeyType0 splitVal = K::getKey0(splitPoint);

                if(K::getKey0((*points)[i]) <= splitVal)
                   // pointsLow.push_back((*points)[i]);
                	pointsLow->push_back((*points)[i]);
                else if(K::getKey0((*points)[i]) > splitVal) {
                   // pointsHigh.push_back((*points)[i]);
                    pointsHigh->push_back((*points)[i]);
                }
            }
            else {
                typename K::KeyType1 splitVal = K::getKey1(splitPoint);

                if(K::getKey1((*points)[i]) <= splitVal)
                    //pointsLow.push_back((*points)[i]);
                	pointsLow->push_back((*points)[i]);
                else if(K::getKey1((*points)[i]) > splitVal) {
                 //   pointsHigh.push_back((*points)[i]);
                    pointsHigh->push_back((*points)[i]);
                }
            }
        }
    }


    /**
     * Less than comparator that uses the oid of the point. This can be passed
     * to std::sort to sort an stl collection.
     */
    template<typename U, typename K>
    inline bool idlt(U* a, U* b) {
        return (K::getOID(a) < K::getOID(b));
    }



    template<typename U, typename K>
    inline int compare0(U* a, U* b) {
        typename K::KeyType0 a0 = K::getKey0(a);
        typename K::KeyType1 a1 = K::getKey1(a);
        typename K::KeyType0 b0 = K::getKey0(b);
        typename K::KeyType1 b1 = K::getKey1(b);


        if(a0 < b0 || (a0 == b0 && a1 < b1)) {
            return -1;
        }
        else if(a0 == b0 && a1 == b1) {
            return 0;
        }
        else {
            return 1;
        }
    }



    template<typename U, typename K>
    inline int compare1(U* a, U* b) {
        typename K::KeyType0 a0 = K::getKey0(a);
        typename K::KeyType1 a1 = K::getKey1(a);
        typename K::KeyType0 b0 = K::getKey0(b);
        typename K::KeyType1 b1 = K::getKey1(b);


        if(a1 < b1 || (a1 == b1 && a0 < b0)) {
            return -1;
        }
        else if(a0 == b0 && a1 == b1) {
            return 0;
        }
        else {
            return 1;
        }
    }



    /**
     * Less than comparator that uses the first attribute. This can be passed
     * to std::sort to sort an stl collection.
     */
    template<typename U, typename K>
    inline bool dim0lt(U* a, U* b) {
        return (compare0<U,K>(a,b) < 0);
    }

    /**
     * Less than comparator that uses the second attribute. This can be passed
     * to std::sort to sort an stl collection.
     */
    template<typename U, typename K>
    inline bool dim1lt(U* a, U* b) {
        return (compare1<U,K>(a,b) < 0);
    }

    template<typename S, typename T>
    inline bool rltlow0(Region2D<S,T>& r1, Region2D<S,T>& r2) {
        return (r1.getLow0() < r2.getLow0());
    }



    /**
     * Returns true iff u1 is closer to target point than u2.
     */
    template<typename U, typename K>
    inline bool closer(U* target, U* u1, U* u2) {
        return (K::distance(target, u1) < K::distance(target, u2));
    }


    /**
     * Less-than comparison operator for pairs. Used to sort lists of
     * pairs. 
     */
    template<typename A, typename B>
    static bool ltPair(std::pair<A*,B*>* o1, std::pair<A*,B*>* o2);



    /**
     * Returns the index dimension with the largest spread (difference between
     * smallest and largest values) given that pointsX are the points sorted
     * on the x dimensions and pointsY are the same points sorted on the y
     * dimension.
     */
    template<typename U, typename K>
    int findMaxSpread(std::vector<U*> & pointsX, std::vector<U*> & pointsY) {
                
        double spread0 = K::getKey0(pointsX[pointsX.size() - 1])
            - K::getKey0(pointsX[0]);
        double spread1 = K::getKey1(pointsY[pointsY.size() - 1])
            - K::getKey1(pointsY[0]);
        

        //printf("spread0: %g\n", fabs(spread0));
        //printf("spread1: %g\n", fabs(spread1));

        if(fabs(spread0) > fabs(spread1))
            return 0;
        else
            return 1;
    }

    template<>
    inline bool ltPair<Point2D, Point2D>(std::pair<Point2D*,Point2D*>* o1,
                                  std::pair<Point2D*,Point2D*>* o2) {
        return ((o1->first->id < o2->first->id)
                ||((o1->first->id == o2->first->id)
                   && (o1->second->id < o2->second->id)));
    }

    template<>
    inline bool ltPair<Point2D, SumAgg2D>(std::pair<Point2D*,SumAgg2D*>* o1,
                                          std::pair<Point2D*,SumAgg2D*>* o2) {
        return ((o1->first->id < o2->first->id)
                ||((o1->first->id == o2->first->id)
                   && (o1->second->getVal() < o2->second->getVal())));
    }
}

#endif /* _INDEX_UTILS_H_ */

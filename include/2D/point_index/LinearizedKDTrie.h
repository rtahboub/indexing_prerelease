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

#ifndef _LINEARIZED_KD_TRIE_H_
#define _LINEARIZED_KD_TRIE_H_

#include "2D/StaticSpatialIndex2D.h"
#include "2D/ZOrder.h"
#include "IndexLog.h"
#include <vector>
#include <bitset>
#include <algorithm>
#include <queue>

namespace PSimIndex {


    template<typename U, typename K, typename A>
    class LinearizedKDTrie : public StaticSpatialIndex2D<U, K, A> {



        //------------------------------------------------------------------------
        // Class Declarations
        //------------------------------------------------------------------------

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
	typedef long zOrder;


        struct Interval {
            uint64_t zLo;
            uint64_t zHi;
            int area; 
            int potential;
            double error;
            uint64_t lowsplitZ;
            uint64_t highsplitZ;


            Interval() : zLo(0UL), zHi(0UL), potential(0), area(0), 
                         error(0), lowsplitZ(0UL), highsplitZ(0UL)
            {}

            Interval(uint64_t zLo, 
                     uint64_t zHi, 
                     int area, 
                     int potential = 0.0, 
                     double error = 0.0, 
                     uint64_t lowsplitZ = 0UL, 
                     uint64_t highsplitZ = 0UL)
                : zLo(zLo), zHi(zHi), area(area), error(error),
                  lowsplitZ(lowsplitZ), highsplitZ(highsplitZ)
            {}

            void split(Interval& left, Interval& right) {
                left.zLo = zLo;
                left.zHi = lowsplitZ;
                right.zLo = highsplitZ;
                right.zHi = zHi;
                left.area = left.zHi - left.zLo + 1;
                right.area = right.zHi - right.zLo + 1;
            }

            friend std::ostream& operator<<(std::ostream &out, 
                                            const Interval& i) {
                return out << "Interval( [" << i.zLo << ", " << i.zHi << "], " 
                           << i.area << ", " << i.potential << ", " 
                           << i.lowsplitZ << ", " << i.highsplitZ << ")";
            }
        };


        class CompareInterval{
        public:
            bool operator()(Interval& I1, Interval&I2) {
                if(I1.potential<I2.potential)
                    return true;
                else
                    return false;
            }
        };

        class CompareLow{
        public:
            bool operator()(Interval& I1, Interval&I2)
            {
                if(I1.zLo>=I2.zLo)
                    return true;
                else
                    return false;
            }

        };


    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:
        static int partition(uint64_t* trieZCodes, U** trieAgents, 
                             int p, int r, uint64_t split);

        static uint64_t median(uint64_t x, uint64_t y, uint64_t z);
        static void quicksort(uint64_t* trieZCodes, U** agents, int p, int r);
        static void quicksort(uint64_t* trieZCodes, U** trieAgents, int size);


        double threshold;
        uint64_t* trieZCodes;
        U** trieAgents;
        int numAgents;
	long Es;
	std::priority_queue<Interval,
                            std::vector<Interval>, 
                            CompareInterval> IntervalPQ;


        void calculatePotential(Interval& parent);
        uint64_t CalculateArea(int x1, int y1, int x2, int y2);

        long Binary(long minZ,long start,long end);
        long TestSubRegion(Interval& interval, IndexCallBack<U>* cb, 
                           RType *r, long start);


        void printInterval(Interval * top);




    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------
    public:
        static const char* getClassName() { return "LinearizedKDTrie"; }
        const char* getInstanceName() { return getClassName(); }

        LinearizedKDTrie(double threshold = 1) : threshold(threshold) {}
        ~LinearizedKDTrie() {}

        void startTick(std::vector<U*>* agents);
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);
        void endTick(bool force = false);

        //Unimplemented methods.
        void aggregateOverRegion(A* aggs, RType* r) {}
        void nearestK(IndexCallBack<U>* cb, U* u, int k) {}
        void afterUpdateAgg(U* u) {}
        void afterInsert(U* u) {}
        void beforeDelete(U* u) {}

    };



    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Implementation Starts Here
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------


    //------------------------------------------------------------------------
    // Private Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    int LinearizedKDTrie<U,K,A>::partition(uint64_t* trieZCodes, 
                                           U** trieAgents, int p, 
                                           int r, uint64_t split) {
        int i = p - 1;
        int j = r + 1;

        while(true) {
            do { 
                j = j - 1; 
            } while(trieZCodes[j] > split);
            do { 
                i = i + 1; 
            } while(trieZCodes[i] < split);
            
            if(i < j) {
                uint64_t tmpZCode = trieZCodes[i]; 
                U* tmpAgent = trieAgents[i];
                
                trieZCodes[i] = trieZCodes[j]; 
                trieAgents[i] = trieAgents[j];
                //queryAgents->at(i) = queryAgents->at(j);
                
                //queries[j] = tmp;
                trieZCodes[j] = tmpZCode;
                trieAgents[j] = tmpAgent;
                //queryAgents->at(j) = tmpAgent;
            }
            else {
                return j;
            }
        }
    }

    template<typename U, typename K, typename A>
    uint64_t LinearizedKDTrie<U,K,A>::median(uint64_t x, 
                                             uint64_t y, 
                                             uint64_t z) {
        uint64_t max, min;
        
        max = std::max(x,y);
        min = std::min(x,y);
        
        if(z > max) 
            return max;
        else {
            if(z < min)
                return min;
            else
                return z;
        }
    }


    template<typename U, typename K, typename A>
    void LinearizedKDTrie<U,K,A>::quicksort(uint64_t* trieZCodes, 
                                            U** trieAgents, 
                                            int p, int r) {
        if(p < r) {
            uint64_t split = median(trieZCodes[p], 
                                    trieZCodes[r], 
                                    trieZCodes[(p+r)/2]);

            int q = partition(trieZCodes, trieAgents, p, r, split);

            quicksort(trieZCodes, trieAgents, p, q);
            quicksort(trieZCodes, trieAgents, q+1, r);
        }
    }

    template<typename U, typename K, typename A>
    void LinearizedKDTrie<U,K,A>::quicksort(uint64_t* trieZCodes, 
                                            U** trieAgents, 
                                            int size) {
        quicksort(trieZCodes, trieAgents, 0, size - 1);
    }



    template<typename U, typename K, typename A>    
    void LinearizedKDTrie<U,K,A>::calculatePotential(Interval& parent) {
        int bitPos = ZOrder::bitDifference(parent.zLo, parent.zHi, 64);
            
        uint64_t highSplitZ = parent.zLo | (1UL << bitPos);
        uint64_t lowSplitZ = parent.zHi & ~(1UL << bitPos);

        for (int i = bitPos - 2; i >= 0; i = i-2) {
            lowSplitZ |= 1UL << i;
            highSplitZ &= ~(1UL << i);
        }

        parent.lowsplitZ = lowSplitZ;
        parent.highsplitZ = highSplitZ;

        int childArea = (parent.zHi - highSplitZ + 1) + (lowSplitZ - parent.zLo + 1);

        parent.potential = parent.area - childArea;
    }

    template<typename U, typename K, typename A>    
    uint64_t LinearizedKDTrie<U,K,A>::CalculateArea(int x1, int y1, 
                                                    int x2, int y2) {
        return ((x2-x1 + 1)*(y2-y1 + 1));
    }

    template<typename U, typename K, typename A>
    long LinearizedKDTrie<U,K,A>::Binary(long minZ,long start,long end) {
        long index= (start+end)/2;		
        if(start>end) {
            if(index > 0) {
                while(trieZCodes[index] == trieZCodes[index-1]) {
                    index--;
                    if(index==0)
                        break;
                }
            }
            return index;
        }
        if(trieZCodes[index] == minZ) {
            if(index > 0) {
                while(trieZCodes[index] == trieZCodes[index-1]) {
                    index--;
                    if(index==0)
                        break;
                }
            }
            return index;
        }
        else if(trieZCodes[index] > minZ) {
            return Binary(minZ, start, index-1);
        }
        else {
            return Binary(minZ, index+1, end);
        }
    }



    template<typename U, typename K, typename A>
    long LinearizedKDTrie<U,K,A>::TestSubRegion(Interval& interval, 
                                                IndexCallBack<U>* cb, 
                                                RType *r, 
                                                long start) {
        long zLo= Binary(interval.zLo, start, numAgents - 1);
		
        for(; trieZCodes[zLo] <= interval.zHi && zLo < numAgents; zLo++) {
            if(r->template containsPoint<U,K>(trieAgents[zLo])
               && trieZCodes[zLo] >= interval.zLo 
               && trieZCodes[zLo] <= interval.zHi) {
                cb->reportPoint(trieAgents[zLo]);
            }
        }  
        return zLo;
    }

    template<typename U, typename K, typename A>
    void LinearizedKDTrie<U,K,A>::printInterval(Interval * top) {
        LOG(DEBUG)<<"\n Interval  : [ " << top->zLo <<", " << top->zHi<<" ]\n"; 	
        //LOG(DEBUG)<<"\n OrgArea :  " << top->originalArea; 
        //LOG(DEBUG)<<"\n OrgWidth :  " << top->orgWidth; 
        LOG(DEBUG)<<"\n Potential :  " << top->potential; 
        LOG(DEBUG)<<"\n Error :  " << top->error; 
    }




    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------


    template<typename U, typename K, typename A>
    void LinearizedKDTrie<U,K,A>::startTick(std::vector<U*>* agents) {
        Es=0;
        numAgents = agents->size();

        trieZCodes = new uint64_t[numAgents];
        trieAgents = new U*[numAgents];

            
        for(int i = 0; i < numAgents; i++) {
            U* agent = agents->at(i);
            trieZCodes[i] = ZOrder::getZOrder(K::quantizeKey0(K::getKey0(agent)),
                                              K::quantizeKey1(K::getKey1(agent)));
            trieAgents[i] = agent;
        }
        quicksort(trieZCodes, trieAgents, numAgents);
    }

    template<typename U, typename K, typename A>
    void LinearizedKDTrie<U,K,A>::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                      RType* r) {

        int queryArea = CalculateArea(K::quantizeKey0(r->getLow0()),
                                      K::quantizeKey1(r->getLow1()),
                                      K::quantizeKey0(r->getHigh0()),
                                      K::quantizeKey1(r->getHigh1()));
                

        uint64_t minZ= ZOrder::getZOrder(K::quantizeKey0(r->getLow0()),
                                         K::quantizeKey1(r->getLow1()));
        uint64_t maxZ= ZOrder::getZOrder(K::quantizeKey0(r->getHigh0()),
                                         K::quantizeKey1(r->getHigh1()));

        if(minZ>maxZ) {
            std::swap(minZ, maxZ);
        }

        Interval left, right;


        int partArea = maxZ - minZ + 1;
        double currError = ((double)(partArea - queryArea) / (double) queryArea);

        Interval whole(minZ, maxZ, partArea);
        calculatePotential(whole);

        IntervalPQ.push(whole);

                    
        while(currError > threshold && !IntervalPQ.empty()) {
                
            //Partition top element of the priority queue;
            Interval candidate = IntervalPQ.top();
            //Stop if we can't improve, regardless of error.
            if(candidate.potential == 0) {
                break;
            }

            IntervalPQ.pop();

            partArea -= candidate.potential;
            currError = ((double)(partArea - queryArea) / (double) queryArea);

            candidate.split(left, right);
            calculatePotential(left);
            calculatePotential(right);

            IntervalPQ.push(left);
            IntervalPQ.push(right);
        }
		
        long startIndex=0;
        while(!IntervalPQ.empty()) {
            Interval top = IntervalPQ.top();
            IntervalPQ.pop();
            startIndex = TestSubRegion(top, cb,r,0);
        }
    }

    template<typename U, typename K, typename A>
    void LinearizedKDTrie<U,K,A>::endTick(bool force) {
        delete [] trieZCodes;
        delete [] trieAgents;

        while(!IntervalPQ.empty())
            IntervalPQ.pop();
    }
}

#endif

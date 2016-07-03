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

#ifndef _STRIPED_SWEEP_H_
#define _STRIPED_SWEEP_H_

#include "2D/join_index/plane_sweep/ListSweep.h"
#include "util/Log.h"

namespace PSimIndex {

    /**
     * TODO: Fix size computation.  
     */
    template <typename U, typename K>
    class StripedSweep : public SweepStructure<U,K> {

    private:

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        int numStripes;
        ListSweep<U,K>** stripes;
        double stripeSize;
        typename K::KeyType1 minY, maxY;
        bool toDel;

        
        int* sizes;
        int size;
        inline int getStripe(typename K::KeyType1 key) { 
//            std::cout << "key: " << key << " getStripe: " << ((key - minY) / stripeSize) << std::endl;
//            DBUTL_ASSERT((int) ((key - minY) / stripeSize) < numStripes);
            return (int) ((key - minY) / stripeSize); 
        }

        char name[200];

    public:

        const char* getName() {
            return name;
        }

        void insert(RType r, U* source) {
            //Won't match any points.
            if(r.getLow1() > maxY || r.getHigh1() < minY) {
                return;
            }

            int lowStripe = std::max(0,getStripe(r.getLow1()));
            int highStripe = std::min(numStripes-1, getStripe(r.getHigh1()));

            // std::cout << "r.low1: " << r.getLow1() << " r.high1: " << r.getHigh1() 
            //           << " getStripe(low): " << getStripe(r.getLow1()) << " getStripe(high): " << getStripe(r.getHigh1())
            //           << " lowStripe: " << lowStripe << " highStripe: " << highStripe << std::endl;

            for(int i = lowStripe; i <= highStripe; i++) {
                stripes[i]->insert(r, source);
                sizes[i] += 1;
                size++;
            }
            //           printSize();
        }

        /**
         * Only remove expired rectangles in the same stripe as limit. This is
         * a kind of lazy deletion. 
         */
        void removeExpired(U* limit) {
            int stripe = getStripe(K::getKey1(limit));


            // std::cout << *limit << std::endl;
            // std::cout << "key: " <<  K::getKey1(limit)<< std::endl;            
            // std::cout << "numStripes: " << numStripes << std::endl;            
            // std::cout << "stripe: " << stripe << std::endl;

            DBUTL_ASSERT(stripe < numStripes);

            //int startSize = stripes[stripe]->getSize();
            stripes[stripe]->removeExpired(limit);
            //int endSize = stripes[stripe]->getSize();

//            std::cout << "startSize: " << startSize << " endSize: " << endSize << std::endl;


            // if(startSize != endSize) {
            //     std::cout << "BLORT!!" << std::endl;
            // }
            //size -= (startSize - endSize);

        }

        void reportOverlap(JoinCallBack<K,U,U>* cb, U* point) {
            int stripe = getStripe(K::getKey1(point));
            stripes[stripe]->reportOverlap(cb, point);
            int oldSize = sizes[stripe];
            int newSize = stripes[stripe]->getSize();
            
            size -= (oldSize - newSize);
            sizes[stripe] = newSize;
        }


        /**
         * Computed lazily, will only be accurate after calling reportOverlap.
         *
         */
        int getSize() {
            return size;
        }

        void print() {
            std::cout << "size: " << size << " ";
            for(int i = 0; i < numStripes; i++) {
                std::cout << stripes[i]->getSize() << " ";
            }
            std::cout << std::endl;
        }
        
        void setMinMax(typename K::KeyType1 minY, typename K::KeyType1 maxY) {
            this->minY = minY;
            this->maxY = maxY;

            stripeSize = (maxY - minY) / (double) numStripes;
            
//            std::cout << "Setting stripeSize = " << stripeSize 
//            << " minY = " << minY 
//            << " maxY = " << maxY << std::endl;
        }

        void clear() {
            for(int i = 0; i < numStripes; i++) {
                stripes[i]->clear();
            }
        }

        StripedSweep(int numStripes, 
                     typename K::KeyType1 minY = K::getKey1Min(),
                     typename K::KeyType1 maxY = K::getKey1Max())
            : numStripes(numStripes) {

            stripes = new ListSweep<U,K>*[numStripes];
            sizes = new int[numStripes];

            for(int i = 0; i < numStripes; i++) {
                stripes[i] = new ListSweep<U,K>();
                sizes[i] = 0;
            }

            setMinMax(minY, maxY);
            
            sprintf(name, "Striped%d", numStripes);

        }

        ~StripedSweep() {
            for(int i = 0; i < numStripes; i++) {
                stripes[i]->clear();
                delete stripes[i];
            }

            delete [] sizes;
            delete [] stripes;
        }
    };
}

#endif /* _STRIPED_SWEEP_H_ */

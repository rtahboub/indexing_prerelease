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


#ifndef _ZORDER_H_
#define _ZORDER_H_

#include <bitset>
#include <iostream>

namespace PSimIndex {


    /**
     * Provides a set of utility functions for computing the space-filling
     * Z-curve that maps a two-dimensional point to one dimension. 
     *
     *
     * @author Narendra Gyanchandani
     * @author Ben Sowell
     */
    class ZOrder {
    public:

        /**
         * Prints an object in binary. 
         *
         * @tparam  T  The type of the object to print. 
         * @param  n  The object to print. 
         */
        template<typename T>
        static void printBits(T n) {
            T i;
            i = ((T)1) << (sizeof(n) * 8 - 1);
            while (i > 0) {
                if (n & i)
                    std::cout << "1";
                else
                    std::cout << "0";
                i >>= 1;
            }
        }


        //static const uint32_t masks[32];



        // static uint64_t getZOrder(uint32_t x, uint32_t y) {

        //     std::bitset<32> bsx(x);
        //     std::bitset<32> bsy(y);
        //     std::bitset<64> bsz;

        //     for(int i=0,j=0;i<32;i++) {
        //         bsz[j++]=bsy[i];
        //         bsz[j++]=bsx[i];
        //     }

        //     return (uint64_t) bsz.to_ulong();
        // }

        /**
         * See
         * http://graphics.stanford.edu/~seander/bithacks.html#InterleaveTableObvious
         * for fast techniques for interleaving bits. Probably could use a
         * table to accelerate further. 
         */
        static uint64_t getZOrder(uint32_t x, uint32_t y) {

            uint64_t ret = 0;

            for(int i = 0; i < 32; i++) {
                ret |= ((y & (1UL << i)) << i) | ((x & (1UL << i)) << (i+1));                
            }

            return ret;
        }






        static int bitDifference(uint64_t zLo, uint64_t zHi, int start) {
            //std::bitset<64> bsLo(zLo);
            //std::bitset<64> bsHi(zHi);

            // for(int i=start-1; i >0 ; i--) {
            //     if((zLo & (1UL << i)) != (zHi & (1UL << i))) {
            //         return i;
            //     }
            // }

            std::bitset<64> bsLo(zLo);
            std::bitset<64> bsHi(zHi);
                
            for(int i=start-1; i >0 ; i--)
            {
                //std::cout << (63-i) << ": " << bsLo[i] << " " << bsHi[i] << std::endl;

                if(bsLo[i]!=bsHi[i])
                    return i;
            }


        }

    };

    // const uint32_t ZOrder::masks[32] = {0x00000001, 0x00000002, 0x00000004, 0x00000008,
    //                                     0x00000010, 0x00000020, 0x00000040, 0x00000080,
    //                                     0x00000100, 0x00000200, 0x00000400, 0x00000800,
    //                                     0x00001000, 0x00002000, 0x00004000, 0x00008000,
    //                                     0x00010000, 0x00020000, 0x00040000, 0x00080000,
    //                                     0x00100000, 0x00200000, 0x00400000, 0x00800000,
    //                                     0x01000000, 0x02000000, 0x04000000, 0x08000000,
    //                                     0x10000000, 0x20000000, 0x40000000, 0x80000000};
    

}

#endif /* _ZORDER_H_ */

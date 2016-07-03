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

#ifndef _QUERY_SORT_H_
#define _QUERY_SORT_H_

#include <iostream>

namespace PSimIndex {
    //template<typename U, typename K>
    
    template<typename U, typename RType, typename DimType, DimType val(RType&)>
    class QuerySort {

    private:

        //typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        // static int partition(RType* queries, std::vector<U*>* queryAgents,
        //                      int p, int r, typename K::KeyType0 split) {

        static int partition(RType* queries, std::vector<U*>* queryAgents,
                             int p, int r, DimType split) {

            int i = p - 1;
            int j = r + 1;

            while(true) {
                do { 
                    j = j - 1; 
//                } while(queries[j].getLow0() > split );
                } while(val(queries[j]) > split );
                do { 
                    i = i + 1; 
//                } while(queries[i].getLow0() < split );
                } while(val(queries[i]) < split );
            
                if(i < j) {
                    RType tmp = queries[i]; 
                    U* tmpAgent = queryAgents->at(i);  
                
                    queries[i] = queries[j]; 
                    queryAgents->at(i) = queryAgents->at(j);
                
                    queries[j] = tmp;
                    queryAgents->at(j) = tmpAgent;

                }
                else {
                    return j;
                }
            }
        }


        // static typename K::KeyType0 median(typename K::KeyType0 x, 
        //                                    typename K::KeyType0 y, 
        //                                    typename K::KeyType0 z) {

        static DimType median(DimType x, DimType y, DimType z) {


//            typename K::KeyType0 max, min;
            DimType max, min;
        
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


    public:

        static void quicksort(RType* queries, 
                              std::vector<U*>* queryAgents, 
                              int p, int r) {

            if(p < r) {
                // typename K::KeyType0 split = median(queries[p].getLow0(),
                //                                     queries[r].getLow0(),
                //                                     queries[(p+r)/2].getLow0());

                DimType split = median(val(queries[p]),
                                       val(queries[r]),
                                       val(queries[(p+r)/2]));
                

                int q = partition(queries, queryAgents, p, r, split);

                quicksort(queries, queryAgents, p, q);
                quicksort(queries, queryAgents, q+1, r);        
            }
        }


        static void quicksort(RType* queries, std::vector<U*>* queryAgents) {
            quicksort(queries, queryAgents, 0, queryAgents->size() - 1);
        }


    };
}

#endif /* _QUERY_SORT_H_ */

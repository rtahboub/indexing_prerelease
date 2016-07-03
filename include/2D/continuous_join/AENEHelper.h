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

#ifndef _AE_HELPER_H_
#define _AE_HELPER_H_

#include "2D/point_index/TPRTree.h"
#include "2D/point_index/TPRegion.h"



namespace PSimIndex {
    template<typename U, typename K, typename A, typename RG, int TPR_m = 1, int TPR_M = 36>
    class AENEHelper : public ContinuousMovingJoin2D<U,K,A,RG> {
    public:
        typedef TPRegion<typename K::KeyType0, typename K::KeyType1> TPRType;
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef TPRTreeNode2D<U,K,TPR_m,TPR_M> NodeType;
        //typedef AENEQueueNode<U,K,RG,TPR_m,TPR_M> QueueNode;
        typedef AENELeafNode<U,K,RG> LeafNode;


        static void joinNodeRegion(NodeType* n, 
                            RType& r, 
                            U* agent, 
                            int queryTick,
                            bool isNodeQuery, 
                            IncrementalCallBack<K,U,U>* icb) {

            for(int i = 0; i < n->getCount(); i++) {
                RType nR = n->mbrs[i].getPosRTypeAt(queryTick);

                if(nR.intersects(&r)) {
                    if(n->isLeaf()) {
                        if(isNodeQuery && K::getOID(n->ptrs[i].data) != K::getOID(agent)) {
                            ////std::cout << "adding 1: " << *(n->ptrs[i].data) << " " << *agent << std::endl;
                            icb->reportPair(n->ptrs[i].data, agent);
                        }
                        else if(K::getOID(n->ptrs[i].data) != K::getOID(agent)){
                            ////std::cout << "adding 2: " << *agent << " " << *(n->ptrs[i].data) << std::endl;
                            icb->reportPair(agent, n->ptrs[i].data);
                        }
                    }
                    else {
                        joinNodeRegion(n->ptrs[i].childPtr, r, agent, 
                                       queryTick, isNodeQuery, icb);
                    }
                }
            }
        }



        /**
         * Joins subtrees rooted at given nodes. This is used to perform the
         * initial join. between the data and queries. 
         */
        static void joinNodes(NodeType* q, NodeType* p, int queryTick, 
                       IncrementalCallBack<K,U,U>* icb) {

            if(q->isLeaf() && !p->isLeaf()) {
                for(int i = 0; i < q->getCount(); i++) {
                    RType qR = q->mbrs[i].getPosRTypeAt(queryTick);
                    joinNodeRegion(p, qR, q->ptrs[i].data, queryTick, false, icb);
                }
            }
            else if(!q->isLeaf() && p->isLeaf()) {
                for(int i = 0; i < p->getCount(); i++) {
                    RType pR = p->mbrs[i].getPosRTypeAt(queryTick);
                    joinNodeRegion(q, pR, p->ptrs[i].data, queryTick, true, icb);
                }
            }
            else {
                for(int i = 0; i < q->getCount(); i++) {
                    for(int j = 0; j < p->getCount(); j++) {
                        RType qR = q->mbrs[i].getPosRTypeAt(queryTick);
                        RType pR = p->mbrs[j].getPosRTypeAt(queryTick);

                        if(qR.intersects(&pR)) {
                            
                            //std::cout << "intersects" << std::endl;
                            if(q->isLeaf() && p->isLeaf()) {
                                if(K::getOID(q->ptrs[i].data) 
                                   != K::getOID(p->ptrs[j].data)) {
                                    icb->reportPair(q->ptrs[i].data, 
                                                   p->ptrs[j].data);
                                }
                            }
                            else { //!q->isLeaf() && !p->isLeaf()
                                joinNodes(q->ptrs[i].childPtr, 
                                          p->ptrs[j].childPtr, 
                                          queryTick, 
                                          icb);
                            }
                        }
                    }
                }
            }
        }


        static bool computeTINF(int tickNum, TPRType& query, TPRType& agent,
                         //int Ts, int Te,
                         bool leftLeaf, bool rightLeaf,
                         int& lowINF, int& highINF, 
                         bool& lowInserted, bool& highInserted,
                         //for testing
                         bool print = false) {  

            int Ts, Te;

            // Use 1000 (which exceeds max tick number for all ticks) as end
            // point for intersection test. 
            // TODO: Make this more general or make it a parameter. 
            bool intersects = TPRType::getIntersectionTime(query, 
                                                           agent, 
                                                           Ts, 
                                                           Te, 
                                                           tickNum, 
                                                           1000);
                                                           //epochNum * genLength);


            if(print) {
                std::cout << "intersects: " << intersects << std::endl;
                std::cout << "tickNum: " << tickNum << " Ts: " << Ts << " Te: " << Te << std::endl;
            }            

            if(!intersects) return false;
            
            if(leftLeaf && rightLeaf) {

                // This will not reinsert a point at a new event generation
                // period.
                if(Ts == tickNum) {
                    lowINF = Te;
                    highINF = -1;
                    lowInserted = false;
                }
                else {
                    DBUTL_ASSERT((Ts > tickNum));

                    lowINF = Ts;
                    lowInserted = true;

                    highINF = Te;
                    highInserted = false;
                }
            }
            //Query rectangle with internal agent node.  This is general TINF
            //from "Time-Parameterized Queries in Spatio-Temporal Databases" by
            //Tao and Papadias, pp. 6.
            else if(leftLeaf && !rightLeaf) {
                if(Ts > tickNum) {
                    lowINF = Ts;
                    highINF = -1;
                    lowInserted = true;
                }
                else if(Ts == tickNum) {
                    lowInserted = false;
                    highInserted = false;

                    //U* qAgent = left->ptrs[lIndex].data;
                    //TPRType qTMBR = left->mbrs[lIndex];
                    //RType query = rg->getRegion(qAgent);
                    
                    //TPRType dataTMBR = right->mbrs[rIndex];
                    RType aMBR = agent.getPosRTypeAt(tickNum);
                    RType qMBR = query.getPosRTypeAt(tickNum);


                    // Query contains agent node. Compute T_PI. 


                    //std::cout << "aMBR: " << aMBR << std::endl;
                    //std::cout << "qaMBR: " << qMBR << std::endl;

                    if(qMBR.contains(&aMBR)) {

                        //std::cout << "tickNum: " << tickNum << " Ts: " << Ts << " Te: " << Te << std::endl;
                        int TPI0, TPI1; 
                        
                        // std::cout << "query.getLow0V(): " << query.getLow0V() << std::endl;
                        // std::cout << "agent.getLow0V(): " << agent.getLow0V() << std::endl;
                        // std::cout << "query.getHigh0V(): " << query.getHigh0V() << std::endl;
                        // std::cout << "agent.getHigh0V(): " << agent.getHigh0V() << std::endl;


                        //dim 0
                        double TiLL = tickNum + ((agent.getLow0At(tickNum) - qMBR.getLow0()) /
                                                  (query.getLow0V() - agent.getLow0V()));
                        
                        double TiRR = tickNum + ((agent.getHigh0At(tickNum) - qMBR.getHigh0()) /
                                                  (query.getHigh0V() - agent.getHigh0V()));
                        

                        
                        // std::cout << "In contains case TiLL = " << TiLL << " TiRR: " << TiRR << std::endl;

                        if(query.getLow0V() == agent.getLow0V()
                           && query.getHigh0V() == agent.getHigh0V()) {
                            TPI0 = -1;
                        }
                        else if((Ts <= TiLL && TiLL <= Te) && (Ts <= TiRR && TiRR <= Te)) {
                            TPI0 = (int)ceil(std::min(TiLL, TiRR));
                        }
                        else if((Ts <= TiLL && TiLL <= Te) && (Ts > TiRR || TiRR > Te)) {
                            TPI0 = (int)ceil(TiLL);
                        }
                        else if((Ts > TiLL || TiLL > Te)&& (Ts <= TiRR && TiRR <= Te)) {
                            TPI0 = (int)ceil(TiRR);
                        }
                        //Neither TiLL nor TiRR contained in [Ts,Te). 
                        else {
                            TPI0 = -1;
                        }

                        // std::cout << "TPI0: " << TPI0 << std::endl;
                        
                        //dim 1
                        TiLL = tickNum + ((agent.getLow1At(tickNum) - qMBR.getLow1()) /
                                          (query.getLow1V() - agent.getLow1V()));
                        
                        TiRR = tickNum + ((agent.getHigh1At(tickNum) - qMBR.getHigh1()) /
                                          (query.getHigh1V() - agent.getHigh1V()));
                        
                        // std::cout << "In contains case TiLL = " << TiLL << " TiRR: " << TiRR << std::endl;

                        if(query.getLow1V() == agent.getLow1V()
                           && query.getHigh1V() == agent.getHigh1V()) {
                            TPI1 = -1;
                        }
                        else if((Ts <= TiLL && TiLL <= Te) && (Ts <= TiRR && TiRR <= Te)) {
                            TPI1 = (int) ceil(std::min(TiLL, TiRR));
                        }
                        else if((Ts <= TiLL && TiLL <= Te) && (Ts > TiRR || TiRR > Te)) {
                            TPI1 = (int) ceil(TiLL);
                        }
                        else if((Ts > TiLL || TiLL > Te)&& (Ts <= TiRR && TiRR <= Te)) {
                            TPI1 = (int) ceil(TiRR);
                        }
                        //Neither TiLL nor TiRR contained in [Ts,Te). 
                        else {
                            TPI1 = -1;
                        }
                        
                        // std::cout << "TPI1: " << TPI1 << std::endl;

                        if(TPI0 == -1 && TPI1 == -1) {
                            lowINF = -1;
                        }
                        else if(TPI0 == -1) {
                            lowINF = TPI1;
                        }
                        else if(TPI1 == -1) {
                            lowINF = TPI0;
                        }
                        else {
                            lowINF = std::min(TPI0, TPI1);
                        }
                            
                        highINF = -1;

                    }
                    // Query intersects but does not contain agent node. 
                    else {
                        lowINF = tickNum;
                        highINF = -1;
                    }
                }
                
            }
            //internal Query mbr with agent node. We know that right hand MBR
            //is a point.
            else if(!leftLeaf && rightLeaf) {

                //std::cout << "HERE" << std::endl;


                if(Ts > tickNum) {
                    lowINF = Ts;
                    highINF = -1;
                    lowInserted = true;
                }
                else {
                    DBUTL_ASSERT((Ts == tickNum));
                    lowINF = tickNum;
                    lowInserted = false;
                    highINF = -1;
                }                
            }

            else if(!leftLeaf && !rightLeaf) {

                lowINF = Ts;
                highINF = -1;
                lowInserted = false;
            }

            return true;
        }







    };


}

#endif /* _AE_HELPER_H_ */

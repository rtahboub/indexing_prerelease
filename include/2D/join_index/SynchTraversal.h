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

#ifndef _SYNCH_TRAVERSAL_H_
#define _SYNCH_TRAVERSAL_H_

#include "2D/StaticSpatialJoin2D.h"
#include "2D/point_index/RTree.h"

#include <list>

namespace PSimIndex {

    enum SynchType {STD, RES, SWEEP};

    template<typename U, typename K, typename A, typename RG>
    class SynchTraversal : public StaticSpatialJoin2D<U, K, A, RG> {

        //--------------------------------------------------------------------
        // Static Constants
        //--------------------------------------------------------------------


        static const int DEFAULT_NODE_SIZE = 8;

        //--------------------------------------------------------------------
        // Class Declarations
        //--------------------------------------------------------------------

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef typename RTree<U,K,A>::Node RNode;

        class MBRIntComp {
        private:
            RType* mbrs;
        public:
            
            bool operator()(int a, int b) const {
                return mbrs[a].getLow1() < mbrs[b].getLow1();
            }
            
            void setMBRs(RType* mbrs) {
                this->mbrs = mbrs;
            }

            MBRIntComp(RType* mbrs = NULL) {
                this->mbrs = mbrs;
            }
        };




    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:
        RTree<U,K,A>* agentTree;
        RType pRect;
        SynchType type;
        int nodeSize;

        

        void joinNodeRegion(JoinCallBack<K,U,U>* cb, RNode* n, 
                            RType& r, U* agent, bool nodeQuery);


        void joinNodes(JoinCallBack<K,U,U>* cb, RNode* p, RNode* q);


        void joinNodesRes(JoinCallBack<K,U,U>* cb, RNode* p, 
                          RNode* q, RType& pRect, RType& qRect);


        void joinNodesPlaneSweep(JoinCallBack<K,U,U>* cb, RNode* p, RNode* q);



    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------

    public:

        static const char* getClassName() { return "SynchTraversal2D"; }
        const char* getInstanceName();


        // TODO: Take MAXENTRIES parameter.
        SynchTraversal(SynchType type = STD, int nodeSize = DEFAULT_NODE_SIZE);
        ~SynchTraversal();


        void startTick(std::vector<U*>* agents);
        void enumerationJoin(JoinCallBack<K,U,U>* cb, 
                             std::vector<U*>* agents, 
                             RG* rg);

        void endTick(bool force = false);


        //Unimplemented methods. 

        void aggregateJoin(JoinCallBack<K,U,A>* cb,
                           std::vector<U*>* agents, 
                           RG* rg) {}

        void nearestKJoin(JoinCallBack<K,U,U>* cb, 
                          std::vector<U*>* agents, 
                          int k) {}

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


    template<typename U, typename K, typename A, typename RG>
    void SynchTraversal<U,K,A,RG>::joinNodeRegion(JoinCallBack<K,U,U>* cb, 
                                                  RNode* n, RType& r, 
                                                  U* agent, bool nodeQuery) {
        for(int i = 0; i < n->count; i++) {
            if(n->mbrs[i].intersects(&r)) {
	      if(n->isLeaf()) {
		if(K::getOID(agent) != K::getOID(n->ptrs[i].data)) {
		  if(nodeQuery)
		    cb->reportPair(n->ptrs[i].data, agent);
		  else
		    cb->reportPair(agent, n->ptrs[i].data);                            
		}
	      }
	      else {
		joinNodeRegion(cb, n->ptrs[i].childPtr, 
			       r, agent, nodeQuery);
	      }
            }
        }
    }


    template<typename U, typename K, typename A, typename RG>
    void SynchTraversal<U,K,A,RG>::joinNodes(JoinCallBack<K,U,U>* cb, 
                                             RNode* p, 
                                             RNode* q) {
        if(p->isLeaf() && !q->isLeaf()) {
            for(int i = 0; i < p->getCount(); i++) {
                joinNodeRegion(cb, q, p->mbrs[i], p->ptrs[i].data, true);
            }
        }
        else if(!p->leaf && q->leaf) {
            for(int i = 0; i < q->count; i++) {
                joinNodeRegion(cb, p, q->mbrs[i], q->ptrs[i].data, false);
            }
        }
        else {

            for(int i = 0; i < q->getCount(); i++) {
                for(int j = 0; j < p->getCount(); j++) {                
                    if(q->mbrs[i].intersects(&p->mbrs[j])) {
                        if(p->isLeaf() && q->isLeaf()) {
                            if(K::getOID(q->ptrs[i].data) 
                               != K::getOID(p->ptrs[j].data)) {

                                U* qPoint = q->ptrs[i].data;

                                cb->reportPair(q->ptrs[i].data, 
                                               p->ptrs[j].data);
                            }
                        }
                        else {  // !p.leaf && !q.leaf
                            joinNodes(cb, p->ptrs[j].childPtr, 
                                      q->ptrs[i].childPtr);
                        }
                    }
                }
            }
        }
    }


    template<typename U, typename K, typename A, typename RG>
    void SynchTraversal<U,K,A,RG>::joinNodesRes(JoinCallBack<K,U,U>* cb, 
                                                RNode* p, RNode* q, 
                                                RType& pRect, RType& qRect) {

        if(p->isLeaf() && !q->isLeaf()) {
            for(int i = 0; i < p->count; i++) {
                joinNodeRegion(cb, q, p->mbrs[i], p->ptrs[i].data, true);
            }
        }
        else if(!p->isLeaf() && q->isLeaf()) {
            for(int i = 0; i < q->count; i++) {
                joinNodeRegion(cb, p, q->mbrs[i], q->ptrs[i].data, false);
            }
        }
        else {
            std::list<int> pList, qList;
            std::list<int>::iterator pIt, qIt;

            for(int i = 0; i < q->count; i++) {
                if(q->mbrs[i].intersects(&pRect)) {
                    qList.push_back(i);
                }
            }
            for(int j = 0; j < p->count; j++) {
                if(p->mbrs[j].intersects(&qRect)) {
                    pList.push_back(j);
                }
            }

            for(qIt = qList.begin(); qIt != qList.end(); qIt++) {
                for(pIt = pList.begin(); pIt != pList.end(); pIt++) {                    
                    int i = *qIt;
                    int j = *pIt;

                    if(q->mbrs[i].intersects(&p->mbrs[j])) {
                        if(p->isLeaf() && q->isLeaf()) {
                            if(K::getOID(q->ptrs[i].data) 
                               != K::getOID(p->ptrs[j].data)) {

                                U* qPoint = q->ptrs[i].data;

                                cb->reportPair(q->ptrs[i].data, 
                                               p->ptrs[j].data);
                            }
                        }
                        else {  // !p.leaf && !q.leaf
                            joinNodesRes(cb, 
                                         p->ptrs[j].childPtr, 
                                         q->ptrs[i].childPtr, 
                                         p->mbrs[j], 
                                         q->mbrs[i]);
                        }
                    }
                }
            }
        }
    }


    template<typename U, typename K, typename A, typename RG>
    void SynchTraversal<U,K,A,RG>::joinNodesPlaneSweep(JoinCallBack<K,U,U>* cb, 
                                                       RNode* p, 
                                                       RNode* q) {

            if(p->isLeaf() && !q->isLeaf()) {
                for(int i = 0; i < p->count; i++) {
                    joinNodeRegion(cb, q, p->mbrs[i], p->ptrs[i].data, true);
                }
            }
            else if(!p->isLeaf() && q->isLeaf()) {
                for(int i = 0; i < q->count; i++) {
                    joinNodeRegion(cb, p, q->mbrs[i], q->ptrs[i].data, false);
                }
            }
            else {
                // Becuase the RTM uses STM bulk loading, we assert that each
                // leaf is already sorted by the y-coordinate. Internal nodes
                // may not be, however, so we need to sort. 


                int* pArray = new int[p->getCount()];
                int* qArray = new int[q->getCount()];

                for(int i = 0; i < p->getCount(); i++) {
                    pArray[i] = i;
                }
                for(int i = 0; i < q->getCount(); i++) {
                    qArray[i] = i;
                }

                //just use array of numbers. Look up mbr and ptrs later?

                if(!p->isLeaf()) {
                    DBUTL_ASSERT((!q->isLeaf()));

                    MBRIntComp m(p->mbrs);

                    std::sort(pArray, (pArray + p->getCount()), m);
                    m.setMBRs(q->mbrs);
                    std::sort(qArray, (qArray + q->getCount()), m);
                }


                int pPtr = 0;
                int qPtr = 0;
                
                while(pPtr != p->count && qPtr != q->count) {

                    RType pRect = p->mbrs[pArray[pPtr]];
                    RType qRect = q->mbrs[qArray[qPtr]];

                    if(pRect.getLow1() < qRect.getLow1()) {

                        for(int q2 = qPtr; q2 < q->count; q2++) {

                            RType qRect2 = q->mbrs[qArray[q2]];
                            

                            if(qRect2.getLow1() > pRect.getHigh1()) {
                                //std::cout << std::endl;
                                break;
                            }


                            U* pData = p->ptrs[pArray[pPtr]].data;
                            U* q2Data = q->ptrs[qArray[q2]].data;

                            typename K::KeyType0 pKey0 = K::getKey0(pData);
                                
                            //report points.
                            if(p->isLeaf()
                               && K::getOID(pData) != K::getOID(q2Data)
                               && pKey0 >= qRect2.getLow0()
                               && pKey0 <= qRect2.getHigh0()) {
                                cb->reportPair(q2Data, pData);
                            }
                            //Not a leaf, so recurse.
                            else if(!p->isLeaf()
                                    && pRect.getLow0() <= qRect2.getHigh0()
                                    && pRect.getHigh0() >= qRect2.getLow0()) {

                                joinNodesPlaneSweep(cb, 
                                                    p->ptrs[pArray[pPtr]].childPtr, 
                                                    q->ptrs[qArray[q2]].childPtr);
                            }
                        }
                        pPtr++;
                    }
                    else {
                        for(int p2 = pPtr; p2 < p->count; p2++) {
                            RType pRect2 = p->mbrs[pArray[p2]];

                            if(pRect2.getLow1() > qRect.getHigh1()) {
                                break; 
                            }


                            U* qData = q->ptrs[qArray[qPtr]].data;
                            U* p2Data = p->ptrs[pArray[p2]].data;

                            typename K::KeyType0 pKey0 = K::getKey0(p2Data);
                                
                            //report points.
                            if(p->isLeaf()
                               && K::getOID(p2Data) != K::getOID(qData)
                               && pKey0 >= qRect.getLow0()
                               && pKey0 <= qRect.getHigh0()) {
                                DBUTL_ASSERT((q->isLeaf()));
                                cb->reportPair(qData, p2Data);
                            }
                            //Not a leaf, so recurse.
                            else if(!p->isLeaf()
                                    && pRect2.getLow0() <= qRect.getHigh0()
                                    && pRect2.getHigh0() >= qRect.getLow0()) {
                                joinNodesPlaneSweep(cb, 
                                                    p->ptrs[pArray[p2]].childPtr, 
                                                    q->ptrs[qArray[qPtr]].childPtr);
                            }
                        }
                        qPtr++;

                    }
                }
                delete [] pArray;
                delete [] qArray;
            }
    }


    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------


    template<typename U, typename K, typename A, typename RG>
    const char* SynchTraversal<U,K,A,RG>::getInstanceName() {
        switch(type) {
        case STD:
            return "SynchTraversal2D";
        case RES:
            return "SynchTraversal2DRes";                
        case SWEEP:
            return "SynchTraversal2DSweep";
        }

    }

    template<typename U, typename K, typename A, typename RG>
    SynchTraversal<U,K,A,RG>::SynchTraversal(SynchType type, int nodeSize) 
        : type(type), nodeSize(nodeSize) {
        agentTree = new RTree<U,K,A>(nodeSize);
    }

    template<typename U, typename K, typename A, typename RG>
    SynchTraversal<U,K,A,RG>::~SynchTraversal() {
        delete agentTree;
    }

    template<typename U, typename K, typename A, typename RG>
    void SynchTraversal<U,K,A,RG>::startTick(std::vector<U*>* agents) {
        agentTree->startTick(agents);
            
        if(type == RES && agents->size() != 0) {
            RNode* root = agentTree->root;
            pRect = RTree<U,K,A>::computeMBR(root);
        }
    }

    template<typename U, typename K, typename A, typename RG>
    void SynchTraversal<U,K,A,RG>::enumerationJoin(JoinCallBack<K,U,U>* cb, 
                                                   std::vector<U*>* agents, 
                                                   RG* rg) {
        if(agents->size() == 0 || agentTree->root == NULL)
            return;


        // Construct RTree on query regions.
        RTree<U,K,A> queryTree(nodeSize);
        std::vector<U*>* queryAgents = new std::vector<U*>(*agents);

        queryTree.bulkLoadQueries(queryAgents, rg);

        RNode* p = agentTree->root;
        RNode* q = queryTree.root;
            
        switch(type) {
        case STD:
            joinNodes(cb, p, q);
            break;

        case RES: 
        {
            RType qRect = RTree<U,K,A>::computeMBR(q);
            joinNodesRes(cb, p, q, pRect, qRect);
            break;
        }
        case SWEEP:
            joinNodesPlaneSweep(cb, p, q);
            break;
        }
            
        queryTree.deleteSubtree(queryTree.root);
        delete queryAgents;
    }


    template<typename U, typename K, typename A, typename RG>
    void SynchTraversal<U,K,A,RG>::endTick(bool force) {
        agentTree->endTick();
    }

}
#endif

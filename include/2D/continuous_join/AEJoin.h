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

#ifndef _AE_JOIN_H_
#define _AE_JOIN_H_

#include <vector>
#include <queue>
//#include <tr1/unordered_set>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include "2D/IndexUtils2D.h"
#include "2D/ContinuousMovingJoin2D.h"
#include "2D/continuous_join/AENEQueue.h"
#include "2D/continuous_join/AENEHelper.h"


namespace PSimIndex {

    template<typename U, typename K, typename A, typename RG, int TPR_m = 1, int TPR_M = 36>
    class AEJoin : public ContinuousMovingJoin2D<U,K,A,RG> {


    public: //Static Methods

        typedef TPRegion<typename K::KeyType0, typename K::KeyType1> TPRType;
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef TPRTreeNode2D<U,K,TPR_m,TPR_M> NodeType;
        typedef AENEQueueNode<U,K,RG,TPR_m,TPR_M> QueueNode;
        typedef AENELeafNode<U,K,RG> LeafNode;

        typedef AENEHelper<U,K,A,RG,TPR_m,TPR_M> AEHelper;

    private:

        RG* rg;

        TPRTree<U,K,A,TPR_m,TPR_M> agentTree;
        TPRTree<U,K,A,TPR_m,TPR_M> queryTree;

        std::vector<U*>* agents;
        std::vector<U*>* queryAgents;
        int epochNum;
        int tickNum;
        int genLength; // Event generation period
        IncrementalCallBack<K,U,U>* icb;        

        // std::queue<typename K::IDType> queryDeleteList;
        // std::queue<typename K::IDType> agentDeleteList;

        // std::queue<U*> queryInsertList;
        // std::queue<U*> agentInsertList;

//        std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > queryDeleteSet;
 //       std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > agentDeleteSet;

//        std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > queryInsertSet;
//        std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > agentInsertSet;

        std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > queryDeleteSet;
        std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > agentDeleteSet;

        std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > queryInsertSet;
        std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> > agentInsertSet;

        AENEQueue<U,K,RG,TPR_m,TPR_M> queue;
        bool firstTick;


//        void addNodeLeafEvents(AENEQueue<U,K,RG,TPR_m,TPR_M>& myQueue,
//                               NodeType* qNode,
//                               LeafNode* aLeaf,
 //                              std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* filterSet) {

        	  void addNodeLeafEvents(AENEQueue<U,K,RG,TPR_m,TPR_M>& myQueue,
        	                               NodeType* qNode,
        	                               LeafNode* aLeaf,
        	                               std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* filterSet) {


            int /*Ts, Te,*/ lowINF, highINF;
            bool lowInserted, highInserted;
            bool addLeft = false;
            bool addRight = false;

            U* agent = aLeaf->u;
            TPRType aTMBR = aLeaf->r;

            for(int i = 0; i < qNode->getCount(); i++) {


                //************************************************************************
                // if(qNode->isLeaf()) {
                //     std::cout << "addNodeLeafEvents: Comparing query agent"
                //         << *qNode->ptrs[i].data << " with ";
                // }
                // else {
                //     std::cout << "addNodeLeafEvents: Comparing node " << *qNode->ptrs[i].childPtr
                //               << " with agent";
                // }
                
                // std::cout << *agent << std::endl;
                //*************************************************************************



                bool sameNode = (qNode->isLeaf() 
                                 && (K::getOID(qNode->ptrs[i].data) 
                                     == K::getOID(agent)));


                //std::cout << "qNode mbr: " << qNode->mbrs[i] << std::endl;




                if(!sameNode && AEHelper::computeTINF(tickNum,
                                                      qNode->mbrs[i], 
                                                      aTMBR,
                                                      qNode->isLeaf(), 
                                                      true, 
                                                      lowINF, 
                                                      highINF, 
                                                      lowInserted, 
                                                      highInserted)) {

                        
                    QueueNode *qn;

                    addLeft = (lowINF != -1);
                    addRight = (highINF != -1);



                    //Filter out queries in filter set. 
                    if(qNode->isLeaf() && 
                       (filterSet == NULL 
                        || filterSet->count(qNode->ptrs[i].data) == 0)) {

                        if(addLeft && lowINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(lowINF, 
                                               lowInserted, 
                                               new LeafNode(qNode->ptrs[i].data, 
                                                            true, 
                                                            tickNum, 
                                                            rg), 
                                               aLeaf);

                            myQueue.push(qn);
                        }
                        if(addRight && highINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(highINF,
                                               highInserted, 
                                               new LeafNode(qNode->ptrs[i].data, 
                                                            true, 
                                                            tickNum, 
                                                            rg), 
                                               aLeaf);
                            myQueue.push(qn);
                        }
                            

                    }
                    else if(!qNode->isLeaf()) {
                        if(addLeft && lowINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(lowINF, lowInserted, 
                                               qNode->ptrs[i].childPtr, 
                                               aLeaf);
                            myQueue.push(qn);
                        }

                        if(addRight && lowINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(highINF, highInserted, 
                                               qNode->ptrs[i].childPtr, 
                                               aLeaf);
                            myQueue.push(qn);
                        }

                    }
                }
            }
        }

        
        /**
         * TODO: This could probably be merged with the previous case, but I
         * think it's easier to reason about separately. 
         */
 /*       void addLeafNodeEvents(AENEQueue<U,K,RG,TPR_m,TPR_M>& myQueue,
                               LeafNode* qLeaf, 
                               NodeType* aNode,
                               std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* filterSet) {
 */
            void addLeafNodeEvents(AENEQueue<U,K,RG,TPR_m,TPR_M>& myQueue,
                                   LeafNode* qLeaf,
                                   NodeType* aNode,
                                   std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* filterSet) {


            	int /*Ts, Te,*/ lowINF, highINF;
            bool lowInserted, highInserted;

            U* qAgent = qLeaf->u;
            TPRType qTMBR = qLeaf->r;



            for(int i = 0; i < aNode->getCount(); i++) {
                //************************************************************************
                // std::cout << "addLeafNodeEvents: Comparing qAgent " << *qAgent << " with ";
                // if(aNode->isLeaf()) {
                //     std::cout << "agent " << *aNode->ptrs[i].data << std::endl;
                // }
                // else {
                //     std::cout << "node " << *aNode->ptrs[i].childPtr << std::endl;
                // }
                //*************************************************************************


                
                bool sameNode = (aNode->isLeaf() 
                                 && (K::getOID(qAgent) 
                                     == K::getOID(aNode->ptrs[i].data)));


                bool TINF = AEHelper::computeTINF(tickNum,
                                                  qTMBR,
                                                  aNode->mbrs[i], 
                                                  true,
                                                  aNode->isLeaf(), 
                                                  lowINF, 
                                                  highINF, 
                                                  lowInserted, 
                                                  highInserted,
                                                  (tickNum == 6 && epochNum == 4 && qAgent->id == 981 && aNode->isLeaf()
                   && aNode->ptrs[i].data->id == 618)
                    );


/*                 if(tickNum == 6 && epochNum == 4 && qAgent->id == 981 && aNode->isLeaf() */
/*                    && aNode->ptrs[i].data->id == 618) { */

/*                     std::cout << "lowINF: " << lowINF << " highINF: " << highINF */
/*                               << " lowInserted: " << lowInserted */
/*                               << " highInserted: " << highInserted << std::endl; */

/*                 } */


                if(!sameNode && TINF) {


                    QueueNode *qn;

                    bool addLeft = (lowINF != -1);
                    bool addRight = (highINF != -1);


                    if(aNode->isLeaf() 
                       && (filterSet == NULL 
                           || filterSet->count(aNode->ptrs[i].data) == 0)) {

                        if(addLeft && lowINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(lowINF, lowInserted, qLeaf, 
                                               new LeafNode(aNode->ptrs[i].data, 
                                                            false, 
                                                            tickNum, 
                                                            rg));
                            myQueue.push(qn);
                        }
                        if(addRight && highINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(highINF, highInserted, qLeaf, 
                                               new LeafNode(aNode->ptrs[i].data, 
                                                            false, 
                                                            tickNum, 
                                                            rg));
                            myQueue.push(qn);
                        }


                    }
                    else if(!aNode->isLeaf()) {

                        if(addLeft && lowINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(lowINF, lowInserted, 
                                               qLeaf,
                                               aNode->ptrs[i].childPtr);
                            myQueue.push(qn);
                        }
                        if(addRight && highINF <= genLength/*nextEpoch*/) {
                            qn = new QueueNode(highINF, highInserted, 
                                               qLeaf,
                                               aNode->ptrs[i].childPtr);
                            myQueue.push(qn);
                        }
                    }
                }
            }
        }


        void addNodePairEvents(AENEQueue<U,K,RG,TPR_m,TPR_M>& myQueue, 
                               NodeType* node1, 
                               NodeType* node2) {
            int /*Ts, Te,*/ lowINF, highINF;
            bool lowInserted, highInserted;
            
            for(int i = 0; i < node1->getCount(); i++) {
                for(int j = 0; j < node2->getCount(); j++) {

                    //************************************************************************
                    // if(node1->isLeaf()) {
                    //     std::cout << "addNodePairEvents: Comparing agent " 
                    //               << *node1->ptrs[i].data << " with ";
                    // }
                    // else {
                    //     std::cout << "addNodePairEvents: Comparing node " 
                    //               << *node1->ptrs[i].childPtr << " with ";
                    // }

                    // if(node2->isLeaf()) {
                    //     std::cout << *node2->ptrs[j].data << std::endl;
                    // }
                    // else {
                    //     std::cout << *node2->ptrs[j].childPtr << std::endl;
                    // }

                    // std::cout << "left mbr: " << node1->mbrs[i] << std::endl;
                    // std::cout << "right mbr: " << node2->mbrs[j] << std::endl;

                    //************************************************************************* 


                    bool sameNode = (node1->isLeaf() && node2->isLeaf() && 
                                     (K::getOID(node1->ptrs[i].data) 
                                      == K::getOID(node2->ptrs[j].data)));




                    if(!sameNode && AEHelper::computeTINF(tickNum,
                                                          node1->mbrs[i], 
                                                          node2->mbrs[j], 
                                                          node1->isLeaf(), 
                                                          node2->isLeaf(), 
                                                          lowINF, 
                                                          highINF, 
                                                          lowInserted, 
                                                          highInserted)) {




                        QueueNode *qn;

                        bool addLeft = (lowINF != -1);
                        bool addRight = (highINF != -1);


/*                         if(node1->isLeaf() && node2->isLeaf()  */
/*                            && K::getOID(node1->ptrs[i].data) == 62 */
/*                            && K::getOID(node2->ptrs[j].data) == 65) { */

/*                             std::cout << "lowINF: " << lowINF << " highINF: " << highINF */
/*                                       << " addLeft: " << addLeft << " addRight: " << addRight */
/*                                       << std::endl; */

//                        }
                        if(node1->isLeaf() && node2->isLeaf()) {


                            if(addLeft && lowINF <= genLength/*nextEpoch*/) {


                                qn = new QueueNode(lowINF, lowInserted,
                                                   new LeafNode(node1->ptrs[i].data,
                                                                true, 
                                                                tickNum, 
                                                                rg),
                                                   new LeafNode(node2->ptrs[j].data, 
                                                                false, 
                                                                tickNum, 
                                                                rg));

                                myQueue.push(qn);
                            }
                            if(addRight && highINF <= genLength/*nextEpoch*/) {
                                qn = new QueueNode(highINF, highInserted,
                                                   new LeafNode(node1->ptrs[i].data,
                                                                true, 
                                                                tickNum, 
                                                                rg),
                                                   new LeafNode(node2->ptrs[j].data, 
                                                                false, 
                                                                tickNum, 
                                                                rg));


                                //std::cout << "\tpushing: " << *qn << std::endl;
                                myQueue.push(qn);
                            }
                        }
                        else if(node1->isLeaf() && !node2->isLeaf()) {

                            if(addLeft && lowINF <= genLength/*nextEpoch*/) {
                                qn = new QueueNode(lowINF, lowInserted, 
                                                   new LeafNode(node1->ptrs[i].data, 
                                                        true, 
                                                        tickNum, 
                                                        rg),
                                                   node2->ptrs[j].childPtr);
                                myQueue.push(qn);
                            }
                            if(addRight && highINF <= genLength/*nextEpoch*/) {
                                qn = new QueueNode(highINF, highInserted, 
                                                   new LeafNode(node1->ptrs[i].data, 
                                                        true, 
                                                        tickNum, 
                                                        rg),
                                                   node2->ptrs[j].childPtr);
                                myQueue.push(qn);
                            }
                        }
                        else if(!node1->isLeaf() && node2->isLeaf()) {
                            
                            if(addLeft && lowINF <= genLength/*nextEpoch*/) {
                                qn = new QueueNode(lowINF, lowInserted, 
                                                   node1->ptrs[i].childPtr, 
                                                   new LeafNode(node2->ptrs[j].data, 
                                                        false, 
                                                        tickNum, 
                                                        rg));
                                myQueue.push(qn);
                            }

                            if(addRight && highINF <= genLength/*nextEpoch*/) {
                                qn = new QueueNode(highINF, highInserted, 
                                                   node1->ptrs[i].childPtr, 
                                                   new LeafNode(node2->ptrs[j].data, 
                                                        false, 
                                                        tickNum, 
                                                        rg));
                                myQueue.push(qn);
                            }
                        }
                        else /* if(!node1->isLeaf() && !node2->isLeaf())*/ {
                            if(addLeft && lowINF <= genLength/*nextEpoch*/) {
                                qn = new QueueNode(lowINF, lowInserted, 
                                                   node1->ptrs[i].childPtr, 
                                                   node2->ptrs[j].childPtr);
                                myQueue.push(qn);
                            }

                            if(addRight && highINF <= genLength/*nextEpoch*/) {
                                qn = new QueueNode(highINF, highInserted, 
                                                   node1->ptrs[i].childPtr, 
                                                   node2->ptrs[j].childPtr);
                                myQueue.push(qn);
                            }
                        }
                    }
                }
            }
        }
        

        /*
         * Algorithm CWQ from "Spatial Queries in Dynamic Environments" by
         * Yufei Tao and Dimitris Papadias, pp. 114. 
         */
//        bool insertNewEvents(LeafNode* leaf, bool isQuery,
//                             std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* filterSet) {

        	bool insertNewEvents(LeafNode* leaf, bool isQuery,
        	                             std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* filterSet) {

            AENEQueue<U,K,RG,TPR_m,TPR_M> tmpQueue;

            if(isQuery) {
                addLeafNodeEvents(tmpQueue, leaf, agentTree.getRoot(), filterSet);
            }
            else {
                addNodeLeafEvents(tmpQueue, queryTree.getRoot(), leaf, filterSet);
            }

            if(tmpQueue.size() == 0) {
                return true;
            }

            while(!tmpQueue.empty()) {
                QueueNode* qnode = tmpQueue.pop();

                if((isQuery && qnode->isRightLeaf)
                   || (!isQuery && qnode->isLeftLeaf)) {
                    
/*                     if(qnode->getLeftAgent()->id == 981 && qnode->getRightAgent()->id == 618) { */
/*                         std::cout << "In insertNewEvents: inserting " << *qnode << std::endl; */
/*                     } */
                    //std::cout << "push1" << std::endl;
                    queue.push(qnode);
                }
                //Right is not leaf. 
                else if(isQuery){
                    //std::cout << "Calling 3: " << *qnode << std::endl;
                    // if(qnode->right.node == NULL) {
                    //     std::cout << "RIGHT IS NULL" << std::endl;
                    // }

                    addLeafNodeEvents(tmpQueue, leaf, qnode->right.node, filterSet);
                    delete qnode;
                }

                //Left is not leaf. 
                else {
                    addNodeLeafEvents(tmpQueue, qnode->left.node, leaf, filterSet);
                    delete qnode;
                }
            }
            return false;
        }


        /*
         * Algorithm CSJ from "Spatial Queries in Dynamic Environments" by
         * Yufei Tao and Dimitris Papadias, pp. 114. 
         *
         */
        void insertAllEvents(/*int stopTime*/) {

            //std::cout << "In insertAllEvents" << std::endl;

            //QueueType tmpQueue;
            AENEQueue<U,K,RG,TPR_m,TPR_M> tmpQueue;

            NodeType* qRoot = queryTree.getRoot();
            NodeType* pRoot = agentTree.getRoot();
            


            addNodePairEvents(tmpQueue, qRoot, pRoot);


            while(!tmpQueue.empty()) {

                QueueNode* qnode = tmpQueue.pop();

                //std::cout << "popping queue entry: " << *qnode << std::endl;

                if(qnode->isLeftLeaf && qnode->isRightLeaf) {

/*                     if(qnode->getLeftAgent()->id == 981 && qnode->getRightAgent()->id == 618) { */
/*                         std::cout << "In insertNewEvents: inserting " << *qnode  */
/*                                   << " epochNum " << epochNum << " tickNum: " << epochNum << std::endl; */
/*                     } */

                    //std::cout << "adding qnode: " << *qnode << std::endl;
                    //std::cout << "push2" << std::endl;
                    queue.push(qnode);
                }
                //When are these cases called??
                else if(qnode->isLeftLeaf) {
                    //std::cout << "Calling 4" << std::endl;
                    addLeafNodeEvents(tmpQueue, qnode->left.leafNode, qnode->right.node, NULL);
                    delete qnode;
                }
                else if(qnode->isRightLeaf) {
                    addNodeLeafEvents(tmpQueue, qnode->left.node, qnode->right.leafNode, NULL);
                    delete qnode;
                }
                else {
                    addNodePairEvents(tmpQueue, qnode->left.node, qnode->right.node);
                    delete qnode;
                }

            }

            //std::cout << "Exiting insertAllEvents" << std::endl;
        }

    public:


        static const char* getClassName() { return "AEJoin"; }
        const char* getInstanceName() { return getClassName(); }
        
        /**
         * Gets and sets the region generator object used by the this index.  
         */
        RG* getRegionGen() const { return rg; }
        void setRegionGen(RG* rg) { 
            this->rg = rg;
        }
        
        virtual IncrementalCallBack<K,U,U>* getCallBack() const { return icb; }
        virtual void setCallBack(IncrementalCallBack<K,U,U>* icb) { 
            this->icb = icb;
        }


        /**
         * Initializes the points and queries for the a new tick. The exact 
         * behavior of this method will depend on the implementation. 
         */
        void startTick(std::vector<U*>* agents, 
                       std::vector<U*>* queryAgents) {


	  //std::cout << "Tick Number: " << tickNum << " epochNum: " << epochNum << std::endl;
	  
            this->agents = agents;
            this->queryAgents = queryAgents;


            agentTree.startTick(agents);
            queryTree.startTickQueries(queryAgents, rg);
            
            // std::cout << "agentTree tickNum: " << agentTree.tickNum << std::endl;

            


            NodeType* q = queryTree.getRoot();
            NodeType* p = agentTree.getRoot();


            // if(tickNum == 6 && epochNum == 4) {            
            //     std::cout << "agentTree..." << std::endl;
            //     agentTree.printSubtree(p);
            // }

           // typename std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >::iterator it;
            typename std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >::iterator it;

            for(it = queryDeleteSet.begin(); it != queryDeleteSet.end(); it++) {
                //std::cout << "Deleting query: " << **it << std::endl;
                icb->deleteLeft(*it);
            }

            queryDeleteSet.clear();

            for(it = agentDeleteSet.begin(); it != agentDeleteSet.end(); it++) {
                //std::cout << "Deleting point: " << **it << std::endl;
                icb->deleteRight(*it);
            }
            agentDeleteSet.clear();


            for(it = queryInsertSet.begin(); it != queryInsertSet.end(); it++) {
                //std::cout << "Inserting query: " << **it << std::endl;



                U* u = *it;




                IndexCallBack<U> cb;
                RType q = rg->getRegion(u);

/*                 if (tickNum == 6 && epochNum == 4 && u->id == 981) { */
/*                     std::cout << "qRegion: " << q << std::endl; */
/*                     std::cout << "agentTree.tickNum: " << agentTree.tickNum << std::endl; */
/*                 } */


            
                agentTree.enumerateOverRegion(&cb, &q); 
                typename IndexCallBack<U>::iterator itt;


                for(itt = cb.begin(); itt != cb.end(); itt++) {
                    if(K::getOID(*itt) != K::getOID(u)) {

/*                         if (tickNum == 6 && epochNum == 4 && u->id == 981) { */
/*                             std::cout << "Adding match " << **itt << std::endl; */
/*                         } */

                        icb->reportPair(u, (*itt));
                    }
                }

                if(tickNum/* % genLength*/ != 0) {
                    LeafNode* ln = new LeafNode(u, true, tickNum, rg);

                    //int oldSize = queue.size();

                    if(insertNewEvents(ln, true, &agentInsertSet)) {
                        delete ln;
                    }

                    // if(queue.size() <= oldSize) {
                    //     delete ln;
                    // }

                }
            }

            

            for(it = agentInsertSet.begin(); it != agentInsertSet.end(); it++) {
                //std::cout << "Inserting agent: " << **it << std::endl;

                U* u = *it;
                
                IndexCallBack<U> cb;
                RType q(K::getKey0(u), K::getKey0(u), K::getKey1(u), K::getKey1(u));

                queryTree.enumerateOverRegion(&cb, &q); 
                typename IndexCallBack<U>::iterator itt;

                for(itt = cb.begin(); itt != cb.end(); itt++) {
                    if(K::getOID(*itt) != K::getOID(u) && !icb->contains((*itt), u)) {
                        icb->reportPair((*itt), u);
                    }
                }


                if(tickNum/* % genLength*/ != 0) {
                    LeafNode* ln = new LeafNode(u, false, tickNum, rg);

                    //int oldSize = queue.size();
                    if(insertNewEvents(ln, false, NULL)){
                        delete ln;
                    }

                    // if(queue.size() <= oldSize) {
                    //     delete ln;
                    // }

                }
            }


            

            queryInsertSet.clear();
            agentInsertSet.clear();


/*             if (tickNum == 6 && epochNum == 4) { */


/*                 std::cout << "icb..." << std::endl; */
/*                 icb->printSorted(); */
            
            
/*                 std::cout << "queue..." << std::endl; */
/*                 queue.printSorted(); */
/*             } */

            // Perform initial join at the beginning of the simulation.
            if(firstTick) {
                AEHelper::joinNodes(q,p,tickNum,icb);
                firstTick = false;
            }

            // Compute new events 
            if(tickNum % genLength == 0) {
                updateJoinResults(false);
                // std::cout << "------" << std::endl;
                // std::cout << "Printing queue..." << std::endl;
                // queue.printSorted();
                insertAllEvents(); //Will this double insert items??
                // std::cout << "Printing queue..." << std::endl;
                // queue.printSorted();
            }
        }
        
        /**
         * Updates the result of the join based on updates made during the 
         * last tick. This method only reports the incremental changes to the 
         * join results. The IncrementalCallBack object contains the following 
         * methods
         *     addResult(U* queryPt, U* dataPt)
         *     removeResult(U* queryPt, U* dataPt)
         */
        void updateJoinResults(bool noInsertGenLength = true) {

            QueueNode* node;// = queue.top();


            //Apply events. 
            while(!queue.empty()) {
                node = queue.pop();

                if(node->isDeleted()) {
                    delete node;
                    continue;
                }

                // std::cout << "Popping node: " << *node << std::endl;


                //std::cout << "node->time: " << node->time << " tickNum: " << tickNum << std::endl;
                DBUTL_ASSERT((node->time >= tickNum));



                if((noInsertGenLength || (tickNum != 0 || node->time != genLength)) && (node->time != tickNum)) {
                    queue.push(node);
                    break;
                }

                // if(node->time != tickNum) {
                //     //std::cout << "push3" << std::endl;
                //     queue.push(node);
                //     break;
                // }


                // std::cout << "Processing node: " << *node << std::endl;

                if(node->inserted) {
                    //std::cout << "inserting" << std::endl;


                    // std::cout << "adding result " << *node->getLeftAgent()  << " "
                    //          << *node->getRightAgent() << std::endl;
                    icb->reportPair(node->getLeftAgent(), node->getRightAgent()); 
                }
                else {
                    // if(/*K::getOID(node->getLeftAgent()) == 791 && */K::getOID(node->getRightAgent()) == 39 && tickNum == 2)
                    // std::cout << "updateJoinResults: deleting result " << *node->getLeftAgent()  << " "
                    //           << *node->getRightAgent() << std::endl;

                    icb->deletePair(node->getLeftAgent(), node->getRightAgent(), tickNum);
                }

                delete node;



            }
        }
                    
        void insertPoint(U* u) {
/*             if(u->id == 618) { */
/*                 std::cout << "Inserting point " << *u << std::endl; */
/*             } */


            agentInsertSet.insert(u);
            agentTree.afterInsert(u);
        }

        /**
         * Also deletes any queries issued by agent u.  
         */
        void deletePoint(U* u) {
/*             if(u->id == 618) { */
/*                 std::cout << "Deleting point " << *u << std::endl; */
/*             } */


            //std::cout << "IN DELETEPOINT " << *u << std::endl;
            agentDeleteSet.insert(u);
            agentTree.beforeDelete(u);
            //queue.deleteRight(K::getOID(u));
        }



        void insertQuery(U* u) {

/*             if(u->id == 981) { */
/*                 std::cout << "Inserting query " << *u << std::endl; */
/*             } */

            // std::cout << "IN INSERTQUERY: " << *u << std::endl;            
            //queryInsertList.push(u);
            queryInsertSet.insert(u);
            queryTree.afterInsertQuery(u, rg);
        }


        void deleteQuery(U* u) {
/*             if(u->id == 981) { */
/*                 std::cout << "Deleting query " << *u << std::endl; */
/*             } */

            // std::cout << "IN DELETEQUERY " << *u << std::endl;
            queryDeleteSet.insert(u);
            queryTree.beforeDelete(u);
            //queue.deleteLeft(K::getOID(u), tickNum);
        }



        /**
         *
         */
        void beforeDelete(U* u) {}


        void afterUpdateAgg(U* u) {}
        void updateVelocity(U* point, 
                            typename K::KeyType0 new0, 
                            typename K::KeyType1 new1, 
                            typename K::KeyType0 newV0, 
                            typename K::KeyType1 newV1) {}
        

       /**
        * Performs any end-of-tick cleanup. If force is true, the index must
        * free all memory it holds. If updates are batched, they should be
        * performed here.
        */
        void endTick(bool force) {

            //icb->printMaps();

            queue.deleteSets(&queryDeleteSet, &agentDeleteSet);

            agentTree.endTick(force);
            queryTree.endTick(force);

            tickNum++;
            if(tickNum % genLength == 0) {
                epochNum++;
                //nextEpoch = nextEpoch + genLength;
                tickNum = 0;
            }

            if(force) {
                //std::cout << "force is true" << std::endl;

                while(!queue.empty()) {
                    QueueNode* node = queue.pop();
                    delete node;
                }
                firstTick = true;
                tickNum = 0;
                epochNum = 0;
                //nextEpoch = epochNum * genLength;
            }

            //std::cout << "exiting end tick " << std::endl;
        }
        
        AEJoin(int genLength = 20) : agentTree(genLength), queryTree(genLength){
        //AEJoin(int genLength = 3) {
            tickNum = 0;
            epochNum = 0;
            //nextEpoch = epochNum * genLength;
            this->genLength = genLength;
            firstTick = true;
        }
        ~AEJoin() {
            delete rg;
            // std::cout << "QueueNode allocated: " << QueueNode::allocated << std::endl;
            // std::cout << "QueueNode deallocated: " << QueueNode::deallocated << std::endl;
            // std::cout << "LeafNode allocated: " << LeafNode::allocated << std::endl;
            // std::cout << "LeafNode deallocated: " << LeafNode::deallocated << std::endl;

        }
    };

    // template<typename U, typename K, typename A, typename RG, int TPR_m, int TPR_M>
    // int AENEJoin<U,K,A,RG,TPR_m,TPR_M>::QueueNode::allocated = 0;
        
    // template<typename U, typename K, typename A, typename RG, int TPR_m, int TPR_M>
    // int AENEJoin<U,K,A,RG,TPR_m,TPR_M>::QueueNode::deallocated = 0;

    // template<typename U, typename K, typename A, typename RG, int TPR_m, int TPR_M>
    // int AENEJoin<U,K,A,RG,TPR_m,TPR_M>::LeafNode::allocated = 0;

    // template<typename U, typename K, typename A, typename RG, int TPR_m, int TPR_M>
    // int AENEJoin<U,K,A,RG,TPR_m,TPR_M>::LeafNode::deallocated = 0;

}

#endif /* _AE_JOIN_H_ */

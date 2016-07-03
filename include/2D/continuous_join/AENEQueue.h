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

#ifndef _AENE_QUEUE_H_
#define _AENE_QUEUE_H_

#include <vector>
#include "2D/Region2D.h"
#include "2D/point_index/TPRegion.h"
#include "2D/point_index/TPRTree.h"

namespace PSimIndex {

// TODO: Not a great implementation at the moment. Think about how to do better. 
// It might make sense to use a balanced binary tree for easier deletion??



    template<typename U, typename K, typename RG>
    struct AENELeafNode {

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef TPRegion<typename K::KeyType0, typename K::KeyType1> TPRType;



        static int allocated;
        static int deallocated;            


        TPRType r;
        U* u;
        int ref; //reference count.


        AENELeafNode(U* u, bool isQuery, int tick, RG* rg) : u(u) {
            //AENELeafNode::allocated += 1;

            // std::cout << "in AENELeafNode: u = "  << *u << std::endl;

            if(!isQuery) {
                r.setPos(RType(K::getKey0(u), K::getKey0(u), K::getKey1(u), K::getKey1(u)));
                r.setVel(RType(K::getV0(u), K::getV0(u), K::getV1(u), K::getV1(u)));
                r.setRefTick(tick);
            }
            else {
                r.setPos(rg->getRegion(u));
                r.setVel(RType(K::getV0(u), K::getV0(u), K::getV1(u), K::getV1(u)));
                r.setRefTick(tick);
            }
            ref = 0;
        }


        AENELeafNode(U* u, TPRType& tr) : u(u){
            //AENELeafNode::allocated += 1;

            r.setPos(tr.getPosRType());
            r.setVel(tr.getVelRType());
            r.setRefTick(tr.getRefTick());
            ref = 0;
        }


        ~AENELeafNode() {
            //AENELeafNode::deallocated += 1;
        }

        friend std::ostream& operator<<(std::ostream &out, const AENELeafNode<U,K,RG>& node) {
            //std::cout << "Entering print" << std::endl;
            out << "AENELeafNode(" << node.r << " point: " << /**node.u*/ K::getOID(node.u)
                << " ref count: " << node.ref << ")";

            //std::cout << "Exiting print" << std::endl;
        }



    };




    template<typename U, typename K, typename RG, int TPR_m, int TPR_M>
    struct AENEQueueNode {

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef TPRegion<typename K::KeyType0, typename K::KeyType1> TPRType;
        typedef TPRTreeNode2D<U,K,TPR_m,TPR_M> NodeType;


        // static int allocated;            
        // static int deallocated;

        /**
         * U* pointer is used in the global priority queue. AENELeafNode*
         * pointer is used when computing events -- it precomputes the
         * TPRType so that the intersection and INF methods don't have to
         * know about the difference between queries and agents. 
         */
        union {
            NodeType* node;
            //U* leaf;
            AENELeafNode<U,K,RG>* leafNode;
        } left, right;


        int time;

        //true if becomes valid, false otherwise. 
        bool inserted; 
        //bool leftIsLeaf, rightIsLeaf;

        //NodeType *left, *right;
        //int lIndex, rIndex;
        bool isLeftLeaf, isRightLeaf;

        bool deleted;
        

        //FOR DEBUGGING
        bool equals(AENEQueueNode<U,K,RG,TPR_m,TPR_M>* other) {
            return ((time == other->time) 
                    && (isLeftLeaf == true)
                    && (isRightLeaf == true)
                    && (isLeftLeaf == other->isLeftLeaf)
                    && (isRightLeaf == other->isRightLeaf)
                    && (K::getOID(getLeftAgent()) == K::getOID(other->getLeftAgent()))
                    && (K::getOID(getRightAgent()) == K::getOID(other->getRightAgent())));
        }

        inline bool isDeleted() {
            return deleted;
        }
        
        inline void markDeleted() {
            deleted = true;
        }

        inline U* getLeftAgent() const {
            DBUTL_ASSERT((isLeftLeaf));
            //return left.leaf;
            return left.leafNode->u;
            //return left->ptrs[lIndex].data;
        }

        inline U* getRightAgent() const {
            DBUTL_ASSERT((isRightLeaf));
            //return right.leaf;
            return right.leafNode->u;
            //return right->ptrs[rIndex].data;
        }


        AENEQueueNode(int time, bool inserted, NodeType* qNode, NodeType* pNode) 
            : time(time), inserted(inserted) {

//                AENEQueueNode::allocated += 1;
            left.node = qNode;
            right.node = pNode;
            isLeftLeaf = isRightLeaf = false;
            deleted = false;
        }


        AENEQueueNode(int time, bool inserted, NodeType* qNode, AENELeafNode<U,K,RG>* aNode)
            : time(time), inserted(inserted) {

            //AENEQueueNode::allocated += 1;

            left.node = qNode;
            right.leafNode = aNode;
            right.leafNode->ref += 1;

            isLeftLeaf = false;
            isRightLeaf = true;
            deleted = false;
        }

        AENEQueueNode(int time, bool inserted, AENELeafNode<U,K,RG>* qNode, NodeType* aNode)
            : time(time), inserted(inserted) {

            //AENEQueueNode::allocated += 1;

            left.leafNode = qNode;
            left.leafNode->ref += 1;

            right.node = aNode;

            isLeftLeaf = true;
            isRightLeaf = false;
            deleted = false;
        }


        AENEQueueNode(int time, bool inserted, 
                  AENELeafNode<U,K,RG>* qNode, 
                  AENELeafNode<U,K,RG>* aNode)
            : time(time), inserted(inserted) {

            //AENEQueueNode::allocated += 1;

            left.leafNode = qNode;
            left.leafNode->ref += 1;

            right.leafNode = aNode;
            right.leafNode->ref += 1;

            isLeftLeaf = isRightLeaf = true;
            deleted = false;
        }


        ~AENEQueueNode() {
            //AENEQueueNode::deallocated += 1;

            if(isLeftLeaf) {
                left.leafNode->ref -= 1;
                //std::cout << "left.leafNode->ref = " << left.leafNode->ref << std::endl;
                if(left.leafNode->ref == 0) {
                    delete left.leafNode;
                }
            }
            if(isRightLeaf) {
                right.leafNode->ref -= 1;
                //std::cout << "right.leafNode->ref = " << right.leafNode->ref << std::endl;
                if(right.leafNode->ref == 0) {
                    delete right.leafNode;
                }
            }
        }

        friend std::ostream& operator<<(std::ostream &out, const AENEQueueNode& node) {
            if(node.isLeftLeaf && node.isRightLeaf) {
                out << "AENEQueueNode(" << node.time << " " << node.inserted; 
                out << " " << *node.left.leafNode->u;
                out << " " << *node.right.leafNode->u << ")";
            }
            else if(node.isLeftLeaf) {
                out << "AENEQueueNode(" << node.time << " " << node.inserted;
                out << " " << *node.left.leafNode;
                out << " " << *node.right.node << ")";
            }
            else if(node.isRightLeaf) {
                out << "AENEQueueNode(" << node.time << " " << node.inserted;
                out << " " << *node.left.node; 
                out << " " << *node.right.leafNode << ")";
            }
            else {
                out << "AENEQueueNode(" << node.time << " " << node.inserted;
                out << " " << *node.left.node; 
                out << " " << *node.right.node << ")";
            }
        }
    };



    template<typename U, typename K, typename RG, int TPR_m, int TPR_M>
    class AENEQueue {

    private:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef TPRegion<typename K::KeyType0, typename K::KeyType1> TPRType;
        typedef TPRTreeNode2D<U,K,TPR_m,TPR_M> NodeType;


        static bool comp(AENEQueueNode<U,K,RG,TPR_m,TPR_M>* a, AENEQueueNode<U,K,RG,TPR_m,TPR_M>* b) {
            return (a->time > b->time);
        }
            
        std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*> myQueue;

        static inline int parent(int i) {
            return (i+1)/2 - 1;
        }
        static inline int left(int i) {
            return 2*i+1;
        }
        static inline int right(int i) {
            return 2*i+2;
        }


        void heapify(int i) {
            AENEQueueNode<U,K,RG,TPR_m,TPR_M>* tmp;

            while(true) {

                int l = left(i);
                int r = right(i);

                int smallest = i;

                if(l < myQueue.size() && myQueue[l]->time < myQueue[i]->time) {
                    smallest = l;
                }
                if(r < myQueue.size() && myQueue[r]->time < myQueue[smallest]->time) {
                    smallest = r;
                }
            
                // std::cout << "smallest: " << smallest << std::endl;
                if(i == smallest) {
                    //std::cout << "breaking i = " << i << " parent = " << parent(i) << std::endl;

                   //print();
                   //std::cout << std::endl;

                   break;
                }
                
                tmp = myQueue[i];
                myQueue[i] = myQueue[smallest];
                myQueue[smallest] = tmp;
                i = smallest;
            } 


        }


        /**
         * Remove the element at position i in the array and fixes up the heap
         * property ("heapify"). Returns the deleted element. 
         */
        AENEQueueNode<U,K,RG,TPR_m,TPR_M>* deleteElement(int i) {

            AENEQueueNode<U,K,RG,TPR_m,TPR_M>* tmp;
            AENEQueueNode<U,K,RG,TPR_m,TPR_M>* key = myQueue[i];

            int starti = i;
            


            //std::cout << "Deleting element " << i << " " << *key << std::endl;
            // print();

            //if(i == 100) {
            //std::cout << "Before Delete " << std::endl;
                //print();
            //std::cout << std::endl << std::endl;
            //}


            if(i == myQueue.size() - 1) {
                myQueue.pop_back();
                return key;
            }

            // // std::cout << "myQueue.size() " << myQueue.size() << std::endl;
            // if(myQueue.size() == 1) {
            //     myQueue.pop_back();                
            //     return key;
            // }



            myQueue[i] = myQueue[myQueue.size() - 1];
            myQueue.pop_back();


            // std::cout << std::endl << "DURING DELETE " << i << " " << myQueue.size() << std::endl;
            // print();
            // std::cout << std::endl;



            //Propogate up the tree. 
            while(i != 0 && myQueue[i]->time < myQueue[parent(i)]->time) {
                tmp = myQueue[i];
                myQueue[i] = myQueue[parent(i)];
                myQueue[parent(i)] = tmp;
                i = parent(i);
            }

            if(i != starti)
                return key;


            //Propogate down the tree. 
            while(true) {



                int l = left(i);
                int r = right(i);

                // if(starti == 100) {
                //     std::cout << "in delete element i = " << i << " l = " << l << " r = " << r
                //               << " queue size = " << myQueue.size() << std::endl;
                // }
                //print();
                //std::cout << std::endl;
                


                int smallest = i;

                if(l < myQueue.size() && myQueue[l]->time < myQueue[i]->time) {
                    smallest = l;
                }
                if(r < myQueue.size() && myQueue[r]->time < myQueue[smallest]->time) {
                    smallest = r;
                }
            
                // std::cout << "smallest: " << smallest << std::endl;
                if(i == smallest) {
                    //std::cout << "breaking i = " << i << " parent = " << parent(i) << std::endl;

                   //print();
                   //std::cout << std::endl;

                   break;
                }
                
                tmp = myQueue[i];
                myQueue[i] = myQueue[smallest];
                myQueue[smallest] = tmp;
                i = smallest;
            } 


            //if(starti == 100){
                //std::cout << std::endl << "AFTER DELETE " << std::endl;
                //print();
                //std::cout << std::endl;
            //}
            
            //DBUTL_ASSERT((validateHeap()));
            return key;

            
        }

        bool validateHeap(int i) {
//            std::cout << "Validating" << std::endl;

            bool leftValid = true;
            if(left(i) < myQueue.size()) {
                leftValid = (validateHeap(left(i)) 
                             && (myQueue[i]->time <= myQueue[left(i)]->time));
                bool leftLocal = (myQueue[i]->time <= myQueue[left(i)]->time);
                if(!leftLocal) {
                    std::cout << "VALIDATION ERROR: i=" << i << " time = " << myQueue[i]->time 
                              << " left child time = " << myQueue[left(i)]->time << std::endl;
                }
            }

            bool rightValid = true;
            if(right(i) < myQueue.size()) {
                rightValid = (validateHeap(right(i))
                              && (myQueue[i]->time <= myQueue[right(i)]->time));


                bool rightLocal = (myQueue[i]->time <= myQueue[right(i)]->time);
                if(!rightLocal) {
                    std::cout << "VALIDATION ERROR: i=" << i << " time = " << myQueue[i]->time 
                              << " right child time = " << myQueue[right(i)]->time << std::endl;
                    
                    //print();
                }
            }

            //std::cout << "i: " << i << " myQueue.size(): " << myQueue.size() << std::endl;
            return (leftValid && rightValid);
        }


    public:
        void push(AENEQueueNode<U,K,RG,TPR_m,TPR_M>* a) {



            //-------DEBUGGING--------
            // bool dup = false;

            // for(int i = 0; i < myQueue.size(); i++) {
            //     if(myQueue[i]->equals(a)) {
            //         std::cout << "INSERTING DUPLICATE INTO QUEUE " << *a << " " << *myQueue[i] << std::endl;
            //         dup = true;
            //     }

            // }
            // if(dup) return;

            //------------------------

            int i = myQueue.size();
            myQueue.push_back(a);
            int key = a->time;

            while(i > 0 && myQueue[parent(i)]->time > key) {
                myQueue[i] = myQueue[parent(i)];
                i = parent(i);
            }

            myQueue[i] = a;

            //DBUTL_ASSERT((validateHeap()));


        }

        AENEQueueNode<U,K,RG,TPR_m,TPR_M>* pop() {

            AENEQueueNode<U,K,RG,TPR_m,TPR_M>* tmp = deleteElement(0);
            //DBUTL_ASSERT((validateHeap()));
            return tmp;

            // std::pop_heap(myQueue.begin(), myQueue.end(), comp);
            // AENEQueueNode<U,K,RG,TPR_m,TPR_M>* ret = myQueue[myQueue.size() - 1];
            // myQueue.pop_back();
            // return ret;
        }

        bool empty() const {
            return myQueue.empty();
        }


        int size() const {
            return (int) myQueue.size();
        }


//        void deleteSets(std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* leftSet,
//                       std::tr1::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* rightSet) {

        void deleteSets(std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* leftSet,
                               std::unordered_set<U*, IDHash<K,U>, IDEq<K,U> >* rightSet) {

            typename std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>::iterator it;
            
            for(it = myQueue.begin(); it != myQueue.end(); it++) {
                AENEQueueNode<U,K,RG,TPR_m,TPR_M>* node = *it;
                U* leftAgent = (node)->getLeftAgent();
                U* rightAgent = (node)->getRightAgent();

                if((leftSet != NULL 
                    && leftSet->count(leftAgent) > 0)
                   || (rightSet != NULL
                       && rightSet->count(rightAgent) > 0)) {
                    node->markDeleted();
                }
            }
        }
        

        //***************************************
        //TMP: TODO: Optimize This. 
        //****************************************
        // void deleteLeft(typename K::IDType uid) {
        //     typename std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>::iterator it = myQueue.begin();

        //     while(it != myQueue.end()) {
        //         U* curr = (*it)->getLeftAgent();

        //         if(K::getOID(curr) == uid) {
        //             myQueue.erase(it);
        //         }
        //         else {
        //             it++;
        //         }
        //     }

        //     for(int i = (myQueue.size()-1) / 2; i >= 0; i--) {
        //         heapify(i);
        //     }

        //     if(!validateHeap()) {
        //         print();
        //     }

        //     DBUTL_ASSERT((validateHeap()));
        // }

        //void deleteRight(typename K::IDType uid) {


            //std::cout << "in deleteRight: " << uid << "queue size = " << myQueue.size() << std::endl;

            // int i = 0;
            // while(true) {
            //     if(i == myQueue.size()) break;

            //     U* curr = myQueue[i]->getRightAgent();
                
            //     if(K::getOID(curr) == uid) {
            //         deleteElement(i);
            //     }
            //     else {
            //         i++;
            //     }
            // }



        //     typename std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>::iterator it = myQueue.begin();

        //     while(it != myQueue.end()) {
        //          U* curr = (*it)->getRightAgent();
        //          if(K::getOID(curr) == uid) {
        //              myQueue.erase(it);
        //          }
        //          else {
        //              it++;
        //          }
        //     }


        //     for(int i = (myQueue.size()-1) / 2; i >= 0; i--) {
        //         heapify(i);
        //     }


        //     DBUTL_ASSERT((validateHeap()));

        // }


        bool validateHeap() {
            return validateHeap(0);
        }

        void print() {
            // std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>* sortedQueue = getSorted();
            // typename std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>::reverse_iterator rit;
            
            // for(rit = sortedQueue->rbegin(); rit != sortedQueue->rend(); rit++) {
            //     std::cout << **rit << std::endl;
            // }

            for(int i = 0; i < myQueue.size(); i++) {
                std::cout << i << ": " << *myQueue[i] << std::endl;
            }

            //delete sortedQueue;
        }


        void printSorted() {
            std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>* sortedQueue = getSorted();
            typename std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>::reverse_iterator rit;
            

            int i = 0; 
            for(rit = sortedQueue->rbegin(); rit != sortedQueue->rend(); rit++) {
                if((*rit)->getLeftAgent()->id == 981)
                    std::cout << i << ": " << **rit << std::endl;

                i++;
            }


            delete sortedQueue;
        }

        std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>* getSorted() {
            std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>* ret
                = new std::vector<AENEQueueNode<U,K,RG,TPR_m,TPR_M>*>(myQueue);

            std::sort(ret->begin(), ret->end(), comp);

//            std::sort_heap(ret->begin(), ret->end(), comp);
            return ret;
        }

    };
}
#endif /* _AENE_QUEUE_H_ */

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


#ifndef _RTREE_H
#define _RTREE_H

#include <map>
#include <vector>
#include "2D/StaticSpatialIndex2D.h"
#include "2D/IndexUtils2D.h"
#include "util/AlignedMem.h"

#include "IndexLog.h"
#include <iostream>
#include <iomanip>

#include "2D/join_index/plane_sweep/QuerySort.h"

namespace PSimIndex {


    template <typename U, typename K, typename A>
    class RTree : public StaticSpatialIndex2D<U, K, A> {


        //------------------------------------------------------------------------
        // Static Constants
        //------------------------------------------------------------------------

        static const int MAX_DEFAULT = 12;
        
        //------------------------------------------------------------------------
        // Class Declarations
        //------------------------------------------------------------------------

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

    public:
        class Node {
        public:
            template <typename U1, typename K1, typename A1, typename RG1> 
            friend class SynchTraversal;


            // Static members for debugging.
            static int nodeVisits; 
            static int nodeAllocs;
            static int nodeDeallocs;

      //  private:
            union Ptr {
                Node* childPtr;
                U* data;
            };

            unsigned short count;
            bool leaf;
            //Pad to word length.
            bool pad;


            // Size formula: 
            // 4 + 32i + 8i

            RType* mbrs;
            Ptr* ptrs;        


        public:

            Node(int max, bool leaf = false);
            ~Node();

            bool isLeaf() const { return leaf; }

            Node* getChild(int i) const {
                DBUTL_ASSERT((!isLeaf()));
                return ptrs[i].childPtr;
            }

            unsigned short getCount() const { return count; }
            RType getMBR(int i) const { return mbrs[i]; }

            void addEntry(const RType& mbr, U* const ptrData);
            void addEntry(const RType& mbr, Node* childPtr);
            void search(RType* r, std::vector<U*>* ans);
            void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);


            friend std::ostream& operator<<(std::ostream &out, 
                                            const Node& node) {
                out << "RTree::Node("
                		<< std::setiosflags(std::ios::fixed)
                    << std::setprecision(2)
                    << (node.isLeaf() ? "leaf" : "internal") << ", "
                    << node.getCount();
            }
        };







    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:

        static RType computeMBR(Node* node);

        int maxEntries; 
        double fillFactor;
        Node* root;
        std::vector<U*>* agents;


        Node* chooseLeaf(Node* node, const RType& mbr);
        std::vector<U*> search(const RType& r);
        void insert(RType mbr, U* const ptrData);
        void deleteSubtree(Node* node);
        void build(const std::vector<Node*>& internalNodes);
        void bulkLoad(std::vector<U*>* agents);

        template<typename RG>
        void bulkLoadQueries(std::vector<U*>* queryPoints, RG* rg);


    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------
    public:

        // SynchTraversal needs special access for traversals. 
        template <typename U1, typename K1, typename A1, typename RG1> 
        friend class SynchTraversal;

        static const char* getClassName() { return "RTree"; }
        const char* getInstanceName() { return getClassName(); }

        RTree(int max = MAX_DEFAULT, double fill = 1.0)
            : maxEntries(max), fillFactor(fill) 
        {}
        ~RTree() {}
        

        void startTick(std::vector<U*>* agents);
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);        
        void endTick(bool force = false);

        void printSubtree(Node* node);


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
    // Node Methods
    //------------------------------------------------------------------------

    template <typename U, typename K, typename A>
    int RTree<U,K,A>::Node::nodeVisits = 0;

    template <typename U, typename K, typename A>
    int RTree<U,K,A>::Node::nodeAllocs = 0;

    template <typename U, typename K, typename A>
    int RTree<U,K,A>::Node::nodeDeallocs = 0;






    template <typename U, typename K, typename A>
    RTree<U,K,A>::Node::Node(int max, bool leaf) {
        nodeAllocs++;
        
        count = 0;
        this->leaf = leaf;

        mbrs = (RType*) AlignedMem::allocate(max * sizeof(RType), 64);
        ptrs = (Ptr*) AlignedMem::allocate(max * sizeof(Ptr), 64);
    }

    template <typename U, typename K, typename A>
    RTree<U,K,A>::Node::~Node() {
        nodeDeallocs++;

        AlignedMem::deallocate(mbrs);
        AlignedMem::deallocate(ptrs);
    }




    template <typename U, typename K, typename A>
    void RTree<U,K,A>::Node::addEntry(const RType& mbr, U* const ptrData) {
        // Default assignment operator should be okay. 
        mbrs[count] = mbr;
        ptrs[count].data = ptrData;
        count++;
    }

    template <typename U, typename K, typename A>
    void RTree<U,K,A>::Node::addEntry(const RType& mbr, Node* childPtr) {
        mbrs[count] = mbr;
        ptrs[count].childPtr = childPtr;
        count++;
    }

    template <typename U, typename K, typename A>
    void RTree<U,K,A>::Node::search(RType* r, std::vector<U*>* ans) {
        if(isLeaf()) {
            // S2: Search leaf node. 
            for(int i = 0; i < count; i++) {
                if (mbrs[i].contains(r)) {
                    ans->push_back(ptrs[i].data);
                }
            }
        }
        else {
            // S1: Search subtrees. 
            for (int i = 0; i < count; i++) {
                if (mbrs[i].contains(r)) {
                    ptrs[i].childPtr->search(r, ans);
                }
            }
        }
    }


    template <typename U, typename K, typename A>
    void RTree<U,K,A>::Node::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                 RType* r) {
        nodeVisits++;

        if (isLeaf()) {
            /* S2: Search leaf node. */
            for (int i = 0; i < count; i++) {
                if (r->intersects(&(mbrs[i]))) {
                    // TODO: templatize the type.
                    int pointID = K::getOID(ptrs[i].data);
                    cb->reportPoint(ptrs[i].data);
                }
            }
        } else {
            /* S1: Search subtrees. */
            for (int i = 0; i < count; i++) {
                if (mbrs[i].intersects(r)) {
                    RType intersection = mbrs[i].getIntersection(r);
                    (ptrs[i].childPtr)->enumerateOverRegion(cb, &intersection);
                }
            }
        }
    }


    //------------------------------------------------------------------------
    // Private Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    typename RTree<U,K,A>::RType RTree<U,K,A>::computeMBR(Node* node) {
        DBUTL_ASSERT((node != NULL));

        typename K::KeyType0 min0 = K::getKey0Max();
        typename K::KeyType0 max0 = K::getKey0Min();
        typename K::KeyType1 min1 = K::getKey1Max();
        typename K::KeyType0 max1 = K::getKey1Min();

        for (int k = 0; k < node->getCount(); k++) {
            RType r = node->getMBR(k);

            if(r.getLow0() < min0) 
                min0 = r.getLow0();
            if(r.getHigh0() > max0) 
                max0 = r.getHigh0();
            if(r.getLow1() < min1) 
                min1 = r.getLow1();
            if(r.getHigh1() > max1) 
                max1 = r.getHigh1();

        }
        RType newR(min0, max0, min1, max1);

       // std::cout<< min0 << " " << max0 << " " << min1 << " "<< max1 <<"\n";

        return newR;
    }


    template<typename U, typename K, typename A>
    typename RTree<U,K,A>::Node* RTree<U,K,A>::chooseLeaf(Node* node, 
                                                        const RType& mbr) {
        if (node->isLeaf()) {
            return node;
        } 
        else {
            Node* childNode = node->chooseSubTree(mbr);
            return chooseLeaf(childNode, mbr);
        }
    }



    template<typename U, typename K, typename A>
    std::vector<U*> RTree<U,K,A>::search(const RType& r) {
        std::vector<U*> ans;
        root->search(r, &ans);
    }

    template<typename U, typename K, typename A>
    void RTree<U,K,A>::insert(RType mbr, U* const ptrData) {

        /* Invoke chooseLeaf to select a leaf node to insert a new entry. */
        Node* leafNode = chooseLeaf(root, mbr);

        LOG(DEBUG) << "Tuan: choosen leaf node: " << *(leafNode) << std::endl;

        /* Insert new entry if leafNode has room. Otherwise, invoke splitNode.
         */
        leafNode->addEntry(mbr, ptrData);

        adjustTree(leafNode);
    }

    template<typename U, typename K, typename A>
    void RTree<U,K,A>::deleteSubtree(Node* node) {
        DBUTL_ASSERT(node != NULL);
        if (node->isLeaf()) {
            delete node;
            return;
        }
        for (int i = 0; i < node->getCount(); i++) {
            deleteSubtree(node->getChild(i));
        }
        delete node;
    }

    template<typename U, typename K, typename A>
    void RTree<U,K,A>::build(const std::vector<Node*>& internalNodes) {
        DBUTL_ASSERT(internalNodes.size() >= 1);
        
        //std::cout << "In build " << internalNodes.size() << std::endl;

        if (internalNodes.size() == 1) {
            root = internalNodes[0];
         //   printSubtree(root);
        } else {
            LOG(DEBUG) << "Building a new level, the  number of nodes is: "
                       << internalNodes.size() << std::endl;
            std::vector<Node*> tempNodes;

            uint32_t numInNode = (uint32_t)(fillFactor * maxEntries);


            int i = 0;
            //for (; i < (int)internalNodes.size() - maxEntries; i += maxEntries) {
            for (; i < (int)internalNodes.size() - (int)numInNode; i += numInNode) {

                //std::cout << "new3" << std::endl;
                Node* inode = new Node(maxEntries);

                for (int k = i; k < i + numInNode; k++) {
                //for (int k = i; k < i + maxEntries; k++) {
                    RType mbr = computeMBR(internalNodes[k]);
                    inode->addEntry(mbr, internalNodes[k]);
                }

                tempNodes.push_back(inode);
                LOG(DEBUG) << "Built node:" << *inode << std::endl;
            }

            bool added = false;

            Node* inode = new Node(maxEntries);

            for ( int k = i; k < internalNodes.size(); k++) {
                RType mbr = computeMBR(internalNodes[k]);
                inode->addEntry(mbr, internalNodes[k]);
                added = true;
            }
            if (added) {
                tempNodes.push_back(inode);
                LOG(DEBUG) << "Built node: " << *inode << std::endl;
            }
            else {
                delete inode;
            }

            build(tempNodes);
        }
    }

    template<typename U, typename K, typename A>
    void RTree<U,K,A>::bulkLoad(std::vector<U*>* agents) {
        // number of leaf level pages.

        // unsigned int P = ceil((double)(agents->size()) / maxEntries); 
        // unsigned int S = sqrt(P);

        uint32_t numInLeaf = (uint32_t)(fillFactor * maxEntries);
        std::cout << "bulk "<<fillFactor << " " << maxEntries << "table size = " << agents->size() << std::endl;
        uint32_t P = ceil((double)(agents->size()) / numInLeaf); 
        uint32_t S = sqrt(P);


        // /*LOG(DEBUG)*/std::cout << "Start bulk loading." << std::endl;
        // /*LOG(DEBUG)*/ std::cout << "\tThe number of data points is: "
        //            << agents->size() << std::endl;
        // /*LOG(DEBUG)*/ std::cout << "\tThe maxEntries is: " << maxEntries << std::endl;
        // std::cout << "\tnumInLeaf is " << numInLeaf << std::endl;
        ///*LOG(DEBUG)*/std::cout << "\tThe number of leaf level pages P is: " << P
        //<< " and S is: " << S << std::endl;

        // sort the points by x-coordinates
        std::sort(agents->begin(), agents->end(), dim0lt<U,K>);

        // roughly number of points in a slice
        //int Sn = S * maxEntries; 
        uint32_t Sn = S * numInLeaf; 


        // std::cout << "Sn " << Sn << std::endl;

        //Sort agents in each slice by y-coordinate. 
        int i = 0;
        for (; i < (int)agents->size() - (int)Sn; i += Sn) {

            // std::cout << "i: " << i << " agents->size(): " << agents->size() << std::endl;
            
            // std::cout << "sorting between " << **(agents->begin() + i)
            //           << " and " << **(agents->begin() + i + Sn)
            //           << std::endl;

            std::sort(agents->begin() + i, agents->begin() + i + Sn, dim1lt<U,K>);
        }

        std::sort(agents->begin() + i, agents->end(), dim1lt<U,K>);

//        std::cout<<"agent size = " <<agents->size()<<"\n";
//        int abc = 0;
//        for(; abc < agents->size(); abc++ ){
//        	std::cout<< K::getOID((*agents)[abc]) << "," <<K::getKey0((*agents)[abc]) << "," << K::getKey1((*agents)[abc]) <<"\n";
//
//
//        }

        std::vector<Node*> tempNodes;

        // build all the leaf nodes
        i = 0;
        //for (; i < (int)agents->size() - maxEntries; i += maxEntries) {
        for (; i < (int)agents->size() - (int)numInLeaf; i += numInLeaf) {
       // 	std::cout<<"i = " << i << "\n";
            Node* leaf = new Node(maxEntries, true);

            // calcuate MBR for each entry
            for (int k = i; k < i + numInLeaf; k++) {
                RType r(K::getKey0((*agents)[k]),
                        K::getKey0((*agents)[k]),
                        K::getKey1((*agents)[k]),
                        K::getKey1((*agents)[k]));


                leaf->addEntry(r, (*agents)[k]);

            }
            tempNodes.push_back(leaf);
//            LOG(DEBUG) << "Built leaf: " << *leaf << std::endl;
        }
        Node* leaf = new Node(maxEntries, true);
        bool added = false;

        // calcuate MBR for each entry
        for (int k = i; k < agents->size(); k++) {

            RType r(K::getKey0((*agents)[k]),
                    K::getKey0((*agents)[k]),
                    K::getKey1((*agents)[k]),
                    K::getKey1((*agents)[k]));

            leaf->addEntry(r, (*agents)[k]);

            added = true;
        }

        //std::cout << "HERE 3" << std::endl;

        if (added) {
            tempNodes.push_back(leaf);
 //           LOG(DEBUG) << "Built leaf: " << *leaf << std::endl;
        }
        else {
            //std::cout << "delete 2";
            delete leaf;
        }

        std::cout<< "number of leaf nodes" <<tempNodes.size() << "\n";

        // build the tree bottom up
        build(tempNodes);
    }



    //TODO: Can I merge this with the point case. Is it bad to sort by the
    //source point coordinates rather the low coordinate of the bounding
    //rectangle??
    template<typename U, typename K, typename A>
    template<typename RG>
    void RTree<U,K,A>::bulkLoadQueries(std::vector<U*>* queryPoints, RG* rg) {
    	std::cout<<"************inside bulk queries\n";
        root = NULL;
        if(queryPoints->size() == 0) return;

        unsigned int P = ceil((double)(queryPoints->size()) / maxEntries); 
        unsigned int S = sqrt(P);

        int numQueries = queryPoints->size();

        RType* queries = new RType[queryPoints->size()];

        for(int i = 0; i < numQueries; i++) {
            queries[i] = rg->getRegion(queryPoints->at(i));
        }

        // Sort all of the rectangles by low x coordinate. 
        QuerySort<U,RType,typename K::KeyType0,RType::low0val>
            ::quicksort(queries, queryPoints);

        // roughly number of points in a slice
        int Sn = S * maxEntries; 

        //Sort queries in each slice by low y-coordinate. 
        int i = 0;
        for (; i < (int)queryPoints->size() - Sn; i += Sn) {
            QuerySort<U,RType,typename K::KeyType1,RType::low1val>::
                quicksort(queries, queryPoints, i, i + Sn - 1);
        }

        QuerySort<U,RType,typename K::KeyType1,RType::low1val>::
            quicksort(queries, queryPoints, i, numQueries - 1);

        //std::sort(agents->begin() + i, agents->end(), dim1lt<U,K>);

        std::vector<Node*> tempNodes;
        Node* leaf;

        // build all the leaf nodes
        i = 0;
        for (; i < (int)queryPoints->size() - maxEntries; i += maxEntries) {

            leaf = new Node(maxEntries, true);
//                 leaf->leaf = true;

            // calcuate MBR for each entry
            for (int k = i; k < i + maxEntries; k++) {
                leaf->addEntry(queries[k], queryPoints->at(k));
            }
            tempNodes.push_back(leaf);

            LOG(DEBUG) << "Built leaf: " << *leaf << std::endl;
        }

        leaf = new Node(maxEntries,true);
//             leaf->leaf = true;

        bool added = false;

        // calcuate MBR for each entry
        for (int k = i; k < queryPoints->size(); k++) {
            leaf->addEntry(queries[k], queryPoints->at(k));
            added = true;
        }

        if (added) {
            tempNodes.push_back(leaf);
            LOG(DEBUG) << "Built leaf: " << *leaf << std::endl;
        }
            
        delete [] queries;

        // build the tree bottom up
        build(tempNodes);
    }




    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    void RTree<U,K,A>::startTick(std::vector<U*>* agents) {
        Node::nodeVisits = 0;
        this->agents = new std::vector<U*>(*agents);
        root = NULL;

        if (!(agents->empty())) {
            bulkLoad(this->agents);
        } 
        //printSubtree(root);
    }

    template<typename U, typename K, typename A>
    void RTree<U,K,A>::enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {
        root->enumerateOverRegion(cb, r);
    }

    template<typename U, typename K, typename A>
    void RTree<U,K,A>::endTick(bool force) {
        delete agents;

        if (root != NULL) {
            deleteSubtree(root);
        }

        Node::nodeAllocs = 0;
        Node::nodeDeallocs = 0;
    }

    template<typename U, typename K, typename A>        
    void RTree<U,K,A>::printSubtree(Node* node) {
        if (node == NULL)  return;

        if (node->isLeaf()) {
            /*LOG(DEBUG)*/std::cout << "\tLeaf: " << *node << ": " << std::endl;
            for (int i = 0; i < node->getCount(); i++) {
//                /*LOG(DEBUG)*/std::cout << "\t\t\t" << *((node->entries[i]).data)
//                                        << " " << node->entries[i].mbr
//                                        << std::endl;

            	std::cout << "\t\t\t" << *((node->ptrs[i]).data)
            	                                        << " " << node->mbrs[i]
            	                                        << std::endl;
            }
            return;
        }

        /*LOG(DEBUG)*/std::cout << "\t" << *node << ": " << std::endl;

        std::vector<Node* > nodes;
        for (int i = 0; i < node->getCount(); i++) {
            //Node* child = (node->entries[i]).childPtr;
        	Node* child = (node->ptrs[i]).childPtr;
            nodes.push_back(child);
            /*LOG(DEBUG)*///std::cout << "\t\t" << *child << "<--->" << (node->entries[i]).mbr << std::endl;
            /*LOG(DEBUG)*/std::cout << "\t\t" << *child << "<--->" << node->mbrs[i] << std::endl;
        }

        /*LOG(DEBUG)*/std::cout << "---------------------------------------------------"
                                << std::endl;

        typename std::vector<Node* >::const_iterator iter;
        for(iter = nodes.begin(); iter != nodes.end(); iter++) {
            printSubtree(*iter);
        }

    }
}

#endif

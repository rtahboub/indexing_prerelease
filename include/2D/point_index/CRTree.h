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


#ifndef _CRTREE_H
#define _CRTREE_H

#include <map>
#include <vector>
#include "2D/StaticSpatialIndex2D.h"
#include "IndexLog.h"
#include <iostream>
#include <iomanip>

#include "util/AlignedMem.h"

namespace PSimIndex {

    template <typename U, typename K, typename A, typename QType = uint16_t>
    class CRTree : public StaticSpatialIndex2D<U, K, A> {



    //------------------------------------------------------------------------
    // Static Constants
    //------------------------------------------------------------------------

    private:

        static const int MAX_DEFAULT = 28;

    //--------------------------------------------------------------------
    // Class Declarations
    //--------------------------------------------------------------------

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef Region2D<QType, QType> QRType;


        class Node {
        public:
            static int nodeVisits; 

            //static unsigned int QLEVEL;

            static QType phi1(double a, double b, double r, uint32_t QLEVEL);
            static QType phi2(double a, double b, double r, uint32_t QLEVEL);
            static QRType computeQR(const RType& ref, const RType& r, uint32_t QLEVEL);

        private:

            union Ptr {
                Node* childPtr;
                U* data;
            };


            // We don't want this to be static so we can support multiple
            // CRTrees. There is room in the header, so we will put it here
            // for now. 
           
            // 4 bytes 
            uint32_t QLEVEL;


            // 4 bytes
            bool leaf;
            uint16_t count;
            bool pad;

            // 32 bytes
            RType referenceMBR;



            // 24 bytes
            char pad2[24];

            // Total header is 64 bytes to help with cache-alignment.
            // Size formula: 
            // 64 + 4 * sizeof(QRType) * i + 8 * i
            QRType* mbrs;
            Ptr* ptrs;
            

        public:

            Node(int max, RType referenceMBR, uint32_t QLEVEL, bool leaf = false);
            ~Node();

            bool isLeaf() const { return leaf; }

            uint16_t getCount() const { return count; }
            QRType getMBR(int i) const { return mbrs[i]; }
            RType getReferenceMBR() const { return referenceMBR; }

            Node* getChild(int i) const;
            U* getData(int i) const;


            void addEntry(const QRType& qrmbr, U* const ptrData);
            void addEntry(const QRType& qrmbr, Node* childPtr);
            void search(QRType* r, std::vector<U*>* ans);
            void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);

            friend std::ostream& operator<<(std::ostream &out, 
                                            const Node& node) {
                out << "CRTree::Node(" << std::setiosflags(std::ios::fixed) 
                    << std::setprecision(2) << ", " << node.getCount() << ", "
                    << node.referenceMBR;
            }
        };





    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:


        uint32_t QLEVEL;
        char name[200];
        Node* root;
        std::vector<U*>* agents;
        int maxEntries;
        double fillFactor;

        std::vector<U*> search(const QRType& r);
        void deleteSubtree(Node* node);
        void build(const std::vector<Node*>& internalNodes);
        void bulkLoad();



    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------
    public:
        static const char* getClassName() { return "CRTree"; }
        const char* getInstanceName() { return name; }

        CRTree(int max = MAX_DEFAULT, double fill = 1.0);
        ~CRTree() {}

        void startTick(std::vector<U*>* agents);
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);
        void endTick(bool force = false);

        void printSubtree(Node* node);

        //unimplemented methods. 
        void aggregateOverRegion(A* aggs, RType* r) {}
        void nearestK(IndexCallBack<U>* cb, U* u, int k) {}
        void afterUpdateAgg(U* u) {}
        void afterInsert(U* u) {}
        void beforeDelete(U* u) {}
    };

    template<typename U, typename K, typename A>
    class CRTreeDefault : public CRTree<U, K, A, uint16_t> {};

    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Implementation Starts Here
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------


    //------------------------------------------------------------------------
    // Node Methods
    //------------------------------------------------------------------------

    // template<typename U, typename K, typename A, typename QType>
    // unsigned int CRTree<U,K,A,QType>::Node::QLEVEL = 255;

    template<typename U, typename K, typename A, typename QType>
    int CRTree<U,K,A,QType>::Node::nodeVisits = 0; 

    template<typename U, typename K, typename A, typename QType>
    QType CRTree<U,K,A,QType>::Node::phi1(double a, 
                                          double b, 
                                          double r, 
                                          uint32_t QLEVEL) {
        if (r <= a) {
            return 0;
        } else if (r >= b) {
            return QLEVEL - 1;
        } else {
            return (QType)(floor(QLEVEL * ((double)(r - a))/(b - a)));
        }
    }

    template<typename U, typename K, typename A, typename QType>
    QType CRTree<U,K,A,QType>::Node::phi2(double a, 
                                          double b, 
                                          double r, 
                                          uint32_t QLEVEL) {
        if (r <= a) {
            return 0;
        } else if (r >= b) {
            return QLEVEL;
        } else {
            return (QType)(ceil(QLEVEL * ((double)(r - a))/(b - a)));
        }
    }

    template<typename U, typename K, typename A, typename QType>
    typename CRTree<U,K,A,QType>::QRType 
    CRTree<U,K,A,QType>::Node::computeQR(const RType& ref, const RType& r, uint32_t QLEVEL) {

        QRType qr(phi1(ref.getLow0(), ref.getHigh0(), r.getLow0(), QLEVEL), 
                  phi2(ref.getLow0(), ref.getHigh0(), r.getHigh0(), QLEVEL),
                  phi1(ref.getLow1(), ref.getHigh1(), r.getLow1(), QLEVEL),
                  phi2(ref.getLow1(), ref.getHigh1(), r.getHigh1(), QLEVEL));

        return qr;
    }

    template<typename U, typename K, typename A, typename QType>    
    CRTree<U,K,A,QType>::Node::Node(int max, RType referenceMBR, 
                                    uint32_t QLEVEL, bool leaf)
        : count(0), QLEVEL(QLEVEL), leaf(leaf), referenceMBR(referenceMBR) { 

        mbrs = (QRType*) AlignedMem::allocate(max * sizeof(QRType), 64);
        ptrs = (Ptr*) AlignedMem::allocate(max * sizeof(Ptr), 64);
    }


    template<typename U, typename K, typename A, typename QType>
    CRTree<U,K,A,QType>::Node::~Node() {
        AlignedMem::deallocate(mbrs);
        AlignedMem::deallocate(ptrs);
    }


    template<typename U, typename K, typename A, typename QType>
    typename CRTree<U,K,A,QType>::Node* 
    CRTree<U,K,A,QType>::Node::getChild(int i) const {
        DBUTL_ASSERT((!isLeaf()));
        DBUTL_ASSERT((i < count));
        return ptrs[i].childPtr;
    }

    template<typename U, typename K, typename A, typename QType>
    U* CRTree<U,K,A,QType>::Node::getData(int i) const {
        DBUTL_ASSERT((isLeaf()));
        DBUTL_ASSERT((i < count));
        return ptrs[i].data;
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::Node::addEntry(const QRType& qrmbr, 
                                             U* const ptrData) {
        mbrs[count] = qrmbr;
        ptrs[count].data = ptrData;
        count++;
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::Node::addEntry(const QRType& qrmbr, 
                                             Node* childPtr) {
        mbrs[count] = qrmbr;
        ptrs[count].childPtr = childPtr;
        count++;
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::Node::search(QRType* r, std::vector<U*>* ans) {
        if (isLeaf()) {
            /* S2: Search leaf node. */
            for (int i = 0; i < count; i++) {
                if (mbrs[i].contains(r)) {
                    ans->push_back(ptrs[i].data);
                }
            }
        } else {
            /* S1: Search subtrees. */
            for (int i = 0; i < count; i++) {
                if (mbrs[i].contains(r)) {
                    (ptrs[i].childPtr)->search(r, ans);
                }
            }
        }
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::Node::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                        RType* r) {
        nodeVisits++;
        QRType qr = computeQR(referenceMBR, *r, QLEVEL);
        if (isLeaf()) {
            /* S2: Search leaf node. */
            for (int i = 0; i < count; i++) {
                if (qr.contains(&(mbrs[i]))) {

                    // TODO: templatize the type.
                    int pointID = (ptrs[i].data)->id;

                    if(r->template containsPoint<U,K>(ptrs[i].data)) {
                        cb->reportPoint(ptrs[i].data);
                    }
                }
            }
        } else {
            /* S1: Search subtrees. */
            for (int i = 0; i < count; i++) {
                if (mbrs[i].intersects(&qr)) {
                    LOG(DEBUG) << "Entry " << i
                               << " intersects the search region" 
                               << std::endl;
                    LOG(DEBUG) << "Enumerate subtree rooted at: " 
                               << *(ptrs[i].childPtr) << std::endl;
                    (ptrs[i].childPtr)->enumerateOverRegion(cb, r);
                }
            }
        }
    }



    //------------------------------------------------------------------------
    // Private Methods
    //------------------------------------------------------------------------


    template<typename U, typename K, typename A, typename QType>
    std::vector<U*> CRTree<U,K,A,QType>::search(const QRType& r) {
        std::vector<U*> ans;
        root->search(r, &ans);
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::deleteSubtree(Node* node) {
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

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::build(const std::vector<Node*>& internalNodes) {
        DBUTL_ASSERT(internalNodes.size() >= 1);

        Node* inode;

        if (internalNodes.size() == 1) {
            root = internalNodes[0];
        } 
        else {
            LOG(DEBUG) << "Building a new level, the  number of nodes is: " 
                       << internalNodes.size() << std::endl;

            std::vector<Node*> tempNodes;


            uint32_t numInNode = (uint32_t)(fillFactor * maxEntries);



            int i = 0;
            //for (; i < (int)internalNodes.size() - maxEntries; i += maxEntries) {
            for (; i < (int)internalNodes.size() - (int)numInNode; i += numInNode) {                    
                RType rMBR = internalNodes[i]->getReferenceMBR();

                // // calculate the reference MBR
                // // TODO: Could be more efficient by doing one pass (see RTree)
                for (int k = i + 1; k < i + numInNode; k++) {
                //for (int k = i + 1; k < i + maxEntries; k++) {
                    rMBR = rMBR.combineMBR(internalNodes[k]->getReferenceMBR());
                }

                inode = new Node(maxEntries, rMBR, QLEVEL);

                // calcuate QRMBR for each entry
                //for (int k = i; k < i + maxEntries; k++) {
                for (int k = i; k < i + numInNode; k++) {
                    RType ref = internalNodes[k]->getReferenceMBR();
                    QRType qrmbr = Node::computeQR(rMBR, ref, QLEVEL);
                    inode->addEntry(qrmbr, internalNodes[k]);
                }

                tempNodes.push_back(inode);
            }
            bool added = false;
            RType rMBR = internalNodes[i]->getReferenceMBR();
            // calculate the reference MBR
            for (int k = i + 1; k < internalNodes.size(); k++) {
                rMBR = rMBR.combineMBR(internalNodes[k]->getReferenceMBR());
            }
                
            inode = new Node(maxEntries, rMBR, QLEVEL);

            // calcuate QRMBR for each entry
            for ( int k = i; k < internalNodes.size(); k++) {
                RType ref = internalNodes[k]->getReferenceMBR();
                QRType qrmbr = Node::computeQR(rMBR, ref, QLEVEL);
                inode->addEntry(qrmbr, internalNodes[k]);
                added = true;
            }
            if (added) {
                tempNodes.push_back(inode);
            }

            build(tempNodes);
        }
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::bulkLoad() {
        // Bulk loading: Sort-Tile-Recursive
        Node* leaf;

        // number of leaf level pages.
        // unsigned int P = ceil((double)(agents->size()) / maxEntries); 
        // unsigned int S = sqrt(P);

        uint32_t numInLeaf = (uint32_t)(fillFactor * maxEntries);
        uint32_t P = ceil((double)(agents->size()) / numInLeaf); 
        uint32_t S = sqrt(P);



        LOG(DEBUG) << "Start bulk loading." << std::endl;
        LOG(DEBUG) << "\tThe number of data points is: " << agents->size()
                   << std::endl;
        LOG(DEBUG) << "\tThe MAXENTRIES is: " << maxEntries << std::endl;
        LOG(DEBUG) << "\tThe number of leaf level pages P is: " << P 
                   << " and S is: " << S << std::endl;

        // sort the points by x-coordinates
        std::sort(agents->begin(), agents->end(), dim0lt<U,K>);

//        int Sn = S * maxEntries; // roughly number of points in a slice
        uint32_t Sn = S * numInLeaf; 

//            std::cout << "Sn: " << Sn << std::endl;

        int i = 0;
        for (; i < (int)agents->size() - (int)Sn; i += Sn) {
            std::sort(agents->begin() + i, 
                      agents->begin() + i + Sn, 
                      dim1lt<U,K>);
        }

        std::sort(agents->begin() + i, agents->end(), dim1lt<U,K>);


        // TODO: implement C-type array
        // or give it a capacity
        std::vector<Node*> tempNodes;


        // build all the leaf nodes

        i = 0;
        //for (; i < (int)agents->size() - maxEntries; i += maxEntries) {
        for (; i < (int)agents->size() - (int)numInLeaf; i += numInLeaf) {
            RType referenceMBR(K::getKey0((*agents)[i]),
                               K::getKey0((*agents)[i]),
                               K::getKey1((*agents)[i]),
                               K::getKey1((*agents)[i]));

            // calculate the reference MBR
            // TODO: Rewrite to avoid allocations.
            //for (int k = i + 1; k < i + maxEntries; k++) {
            for (int k = i; k < i + numInLeaf; k++) {
                RType r(K::getKey0((*agents)[k]),
                        K::getKey0((*agents)[k]),
                        K::getKey1((*agents)[k]),
                        K::getKey1((*agents)[k]));

                referenceMBR = referenceMBR.combineMBR(r);
            }
                
            leaf = new Node(maxEntries, referenceMBR, QLEVEL, true);

            // calcuate QRMBR for each entry
            //for (int k = i; k < i + maxEntries; k++) {
            for (int k = i; k < i + numInLeaf; k++) {
                RType r(K::getKey0((*agents)[k]),
                        K::getKey0((*agents)[k]),
                        K::getKey1((*agents)[k]),
                        K::getKey1((*agents)[k]));

                QRType qrmbr = Node::computeQR(referenceMBR, r, QLEVEL);


                leaf->addEntry(qrmbr, (*agents)[k]);

            }
            tempNodes.push_back(leaf);
        }

        //CRTreeNode2D<U, K, QType>* leaf = new CRTreeNode2D<U, K, QType>(maxEntries);
        RType referenceMBR(K::getKey0((*agents)[i]),
                           K::getKey0((*agents)[i]),
                           K::getKey1((*agents)[i]),
                           K::getKey1((*agents)[i]));
        bool added = false;
        // calculate the reference MBR
        for (int k = i + 1; k < agents->size(); k++) {
            RType r(K::getKey0((*agents)[k]),
                    K::getKey0((*agents)[k]),
                    K::getKey1((*agents)[k]),
                    K::getKey1((*agents)[k]));

            referenceMBR = referenceMBR.combineMBR(r);
        }

        leaf = new Node(maxEntries, referenceMBR, QLEVEL, true);

        // calcuate QRMBR for each entry
        for (int k = i; k < agents->size(); k++) {
            RType r(K::getKey0((*agents)[k]),
                    K::getKey0((*agents)[k]),
                    K::getKey1((*agents)[k]),
                    K::getKey1((*agents)[k]));

            QRType qrmbr = Node::computeQR(referenceMBR, r, QLEVEL);
            leaf->addEntry(qrmbr, (*agents)[k]);

            added = true;
        }

        if (added) {
            tempNodes.push_back(leaf);
        }

        // build the tree bottom up
        build(tempNodes);
    }


    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A, typename QType>
    CRTree<U,K,A,QType>::CRTree(int max, double fill) {
        maxEntries = max;
        fillFactor = fill;
        QLEVEL = (unsigned int)(std::pow(2.0, (double)(sizeof(QType)*8))) - 1;

        //Node::QLEVEL = QLEVEL;
        
        // The template parameter will be part of the name.
        sprintf(name, "%s%d", getClassName(), (int)sizeof(QType));
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::startTick(std::vector<U*>* agents) {
        Node::nodeVisits = 0;
        this->agents = agents;
        root = NULL;

        if (!(agents->empty())) {
            bulkLoad();
        }
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                  RType* r) {
        root->enumerateOverRegion(cb, r);
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::endTick(bool force) {
        if ( root != NULL) {
            deleteSubtree(root);
        }
    }

    template<typename U, typename K, typename A, typename QType>
    void CRTree<U,K,A,QType>::printSubtree(Node* node) {
        if (node == NULL)
            return;

        if (node->isLeaf()) {
            std::cout << "\tLeaf: " << *node << ": " << std::endl;
            for (int i = 0; i < node->getCount(); i++) {
                std::cout << "\t\t\t" << *((node->getData(i))) << std::endl;
            }
            return;
        }

        std::cout << "\t" << *node << ": " << std::endl;

        std::vector<Node* > nodes;
        for (int i = 0; i < node->getCount(); i++) {
            Node* child = node->getChild(i);
            nodes.push_back(child);
            std::cout << "\t\t" << *child << "<--->" << node->getMBR(i)
                      << std::endl;
        }

        std::cout << "---------------------------------------------------"
                   << std::endl;

        typename std::vector<Node* >::const_iterator iter;
        for(iter = nodes.begin(); iter != nodes.end(); iter++) {
            printSubtree(*iter);
        }
    }
}


#endif

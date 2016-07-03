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


#ifndef _OPTIMIZED_STRIPES_H_
#define _OPTIMIZED_STRIPES_H_

//#include <tr1/unordered_map>
#include <unordered_map>
#include "2D/MovingSpatialIndex2D.h"
#include "2D/IndexUtils2D.h"

#define DEBUG_COUNT
#undef DEBUG_STRIPES

namespace PSimIndex {
#ifdef DEBUG_COUNT
    std::vector<int> tree0_size;
    std::vector<int> tree1_size;
    std::vector<int> numInternalNodeVisits;
    std::vector<int> numLeafNodeVisits;
    std::vector<int> numReportAll;

#endif

    std::map<int, uint32_t> levelMap;

    /**
     * OptimizedStripes Base Node.
     */
    template <typename U, typename K>
    class OptimizedStripesNode {
    public:


#ifdef DEBUG_COUNT
        static int numLeafVisits;
        static int numInternalVisits;
        static int numReport;
#endif


#ifdef DEBUG_STRIPES
        static std::vector<U*> tuanTBD;
        static std::vector<U*> updatedPoints;
#endif

        /**
         * A grid encodes information about the quadrant corresponding to a node,
         * in each of the dual planes.
         * In this implementation, we simply indice the quadrant by the lower
         * vertex of the quadrant.
         */
        struct Grid {
            double vx, px;
            double vy, py;
            Grid() {
                vx = vy = 0;
                px = py = 0;
            }
            Grid( double vx, double px, double vy, double py) :
                vx(vx), px(px), vy(vy), py(py) {}
        };

        OptimizedStripesNode() {
            level = 0;
            size = 0;
        }

        OptimizedStripesNode(int level) :
            level(level), size(0) {}


        OptimizedStripesNode(double vx, double px, double vy, double py) {
            level = 0;
            size = 0;

            grid.vx = vx;
            grid.px = px;
            grid.vy = vy;
            grid.py = py;
        }

        void setGrid(double vx, double px, double vy, double py) {
            grid.vx = vx;
            grid.px = px;
            grid.vy = vy;
            grid.py = py;
        }

        uint16_t level;  // non-zero: indicates the level of the non-leaf node,
                         // zero: indicates this node is a leaf node

        uint32_t size;  // non-leaf node: indicates total number of actual data entries stored
                        // in all the leaf nodes in all the leaf nodes in the subtree
                        // below this non-leaf node.
                        // leaf-node: indicates total number of data points stored
                        // in this leaf node.

        Grid grid;

        virtual ~OptimizedStripesNode() {};
    };

#ifdef DEBUG_STRIPES
    template <typename U, typename K>
    std::vector<U*> OptimizedStripesNode<U, K>::tuanTBD;
    template <typename U, typename K>
    std::vector<U*> OptimizedStripesNode<U, K>::updatedPoints;
#endif

#ifdef DEBUG_COUNT
    template <typename U, typename K>
    int OptimizedStripesNode<U, K>::numLeafVisits = 0;

    template <typename U, typename K>
    int OptimizedStripesNode<U, K>::numInternalVisits = 0;

    template <typename U, typename K>
    int OptimizedStripesNode<U, K>::numReport = 0;


#endif

    /**
     * OptimizedStripes Leaf Node.
     */
    template <typename U, typename K>
    class OptimizedStripesLNode : public OptimizedStripesNode<U, K> {
    public:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        static const int MAXENTRIES;

        // For debugging
        static int nextlid;
        int lid;

        struct TransformedPoint {
            double transVx;
            double transVy;
            double pxRef;
            double pyRef;

            TransformedPoint() {
                transVx = transVy = 0;
                pxRef = pyRef = 0;
            }

            TransformedPoint(double vx, double vy, double px, double py) :
                transVx(vx), transVy(vy), pxRef(px), pyRef(py) {}
        };


    public:

        OptimizedStripesLNode(int level = 0) : OptimizedStripesNode<U, K>(level) {
            entries = new U*[MAXENTRIES];
            transEntries = new TransformedPoint*[MAXENTRIES];
            lid = nextlid++;
        }

        ~OptimizedStripesLNode() {
            delete [] entries;
            delete [] transEntries;
        }


        void printQuads(FILE* gp, bool xDim, double SL_P, double SL_V) {

            if(xDim) {
                double ll_px = OptimizedStripesNode<U, K>::grid.px;
                double ll_vx = OptimizedStripesNode<U, K>::grid.vx;

                double ur_px = ll_px + SL_P;
                double ur_vx = ll_vx + SL_V;


                //fprintf(gp, "%f %f %f %f\n", ll_vx, ll_px, ur_vx, ur_px);
                fprintf(gp, "set obj %d rect from %d,%d to %d,%d front fs empty border rgb \"red\"\n", 
                        (lid+1), (int)ll_vx, (int)ll_px, (int)ur_vx, (int)ur_px);


                //std::cout << lid << " " << ll_vx  << " " <<  ll_px << " " << ur_vx << " " << ur_px << std::endl;
            }
            else {
                double ll_py = OptimizedStripesNode<U, K>::grid.py;
                double ll_vy = OptimizedStripesNode<U, K>::grid.vy;

                double ur_py = ll_py + SL_P;
                double ur_vy = ll_vy + SL_V;

                //fprintf(gp, "%f %f %f %f\n", ll_vy, ll_py, ur_vy, ur_py);
                //fprintf(gp, "set obj %d rect from %d,%d to %d,%d\n", 
                //(lid + 1), (int)ll_vy, (int)ll_py, (int)ur_vy, (int)ur_py);


                //std::cout << lid<< " " << ll_vy  << " " <<  ll_py << " " << ur_vy << " " << ur_py << std::endl;
            }


        }

        bool isFull() const {
            return OptimizedStripesNode<U, K>::size == MAXENTRIES;
        }

        bool isEmpty() const {
            return OptimizedStripesNode<U, K>::size == 0;
        }

        void insertPoint(U* point, double transVx, double transVy, double pxRef, double pyRef) {
            DBUTL_ASSERT((OptimizedStripesNode<U, K>::size) < MAXENTRIES);

            entries[OptimizedStripesNode<U, K>::size] = point;
            transEntries[OptimizedStripesNode<U, K>::size] = new TransformedPoint(transVx, transVy, pxRef, pyRef);
            OptimizedStripesNode<U, K>::size++;
        }

        /**
         * Function returns true if the point is found (and then deleted)
         * otherwise, returns false.
         * This function is called from a leaf node.
         */
        bool deletePoint(typename K::IDType oid) {
            bool found = false;
            int i = 0;
            for (; i < OptimizedStripesNode<U, K>::size; i++) {
                // Tuan: since we regenerated the old point, we have to test equality using oid
                if (K::getOID(entries[i]) == oid) {
                    found = true;
                    LOG(DEBUG) << "Tuan: found the point in the leaf node." << std::endl;
                    break;
                }
            }

            // TODO: delete transEntries[i]
            i++;
            for (; i < OptimizedStripesNode<U, K>::size; i++) {
                entries[i-1] = entries[i];
                transEntries[i-1] = transEntries[i];
            }

            if (found) {
                OptimizedStripesNode<U, K>::size--;
                LOG(DEBUG) << "\tRemoved the point and rearranged to fill the gap for that point." << std::endl;
            }

            return found;
        }

        // TODO: use function containsPoint from Region2D.h instead
        bool containsPoint(const RType* r, U* point) {
            return ((r->getLow0() <= K::getKey0(point))
                    && (r->getHigh0() >= K::getKey0(point))
                    && (r->getLow1() <= K::getKey1(point))
                    && (r->getHigh1() >= K::getKey1(point)));

        }

        /**
         * Function enumerates all the data points stored in this leaf node.
         */
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {
            //std::cout << "visiting leaf: " << lid << std::endl;


            if(levelMap.count(OptimizedStripesNode<U, K>::level) == 0) {
                levelMap[OptimizedStripesNode<U, K>::level] = 0;
            }
            levelMap[OptimizedStripesNode<U, K>::level]++;


#ifdef DEBUG_COUNT
            OptimizedStripesNode<U, K>::numLeafVisits++;
#endif
            for (int i = 0; i < OptimizedStripesNode<U, K>::size; i++) {
                U* point  = entries[i];
                if (containsPoint(r, point)) {
                    //std::cout << "Tuan: reporting point: " << *(entries[i]) << std::endl;
                     cb->reportPoint(entries[i]);

#ifdef DEBUG_STRIPES
                     OptimizedStripesNode<U, K>::tuanTBD.push_back(entries[i]);
#endif
                }
            }
        }

        U** entries;
        TransformedPoint** transEntries;


        /**
         * Prints a leaf node to out stream using the format
         *    OptimizedStripesLNode(id, level, {grid}, size)
         *    where {grid} is displayed as {(vx,px),(vy,py)}
         */
        friend std::ostream& operator<<(std::ostream &out, 
                                        const OptimizedStripesLNode<U, K>& node) {

            //std::cout << "in print leaf node" << std::endl;
            
            //out << "OptimizedStripesLNode";

            out << "OptimizedStripesLNode(" 
                << std::setiosflags(std::ios::fixed) << std::setprecision(2)
                << node.lid << ", " << node.level << ", " << node.size << ", "
                << "{(" << node.grid.vx << "," << node.grid.px << "), (" 
                << node.grid.vy << "," << node.grid.py << ")})" << std::endl;
            

            // // out << "\t\t\t\t" << *(node.entries[0])
            // //     << "\t\t\t\t" << "TransformedPoint("
            // //     << (node.transEntries[0])->transVx << ", "
            // //     << (node.transEntries[0])->transVy << ", "
            // //     << (node.transEntries[0])->pxRef << ", "
            // //     << (node.transEntries[0])->pyRef << ")"
            // //     << std::endl;

            for (int i = 0; i <node.size; i++) {
                out << "\t\t\t\t" << *(node.entries[i]) 
                    // << "\t\t\t\t" << "TransformedPoint("
                    // << (node.transEntries[i])->transVx << ", "
                    // << (node.transEntries[i])->transVy << ", "
                    // << (node.transEntries[i])->pxRef << ", "
                    // << (node.transEntries[i])->pyRef << ")"
                    << std::endl;
            }
        }

    };
    template <typename U, typename K>
    const int OptimizedStripesLNode<U, K>::MAXENTRIES = 4;
    template <typename U, typename K>
    int OptimizedStripesLNode<U, K>::nextlid = 0;

    /**
     * OptimizedStripes Internal(non-leaf) node.
     */
    template <typename U, typename K>
    class OptimizedStripesINode : public OptimizedStripesNode<U, K> {
    public:

        /**
         * This structure is used primarily by the pointMap.
         * It keeps track of the parent node and the its child index
         * pointing to the leaf node storing a particular data point.
         */
        struct ChildNodeIndex {
            OptimizedStripesINode<U, K>* parent;
            int childIndex;

            ChildNodeIndex() {
                parent = NULL;
                childIndex = 0;
            }

            ChildNodeIndex(OptimizedStripesINode<U, K>* p, int ci) {
                parent = p;
                childIndex = ci;
            }
        };

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        // Tuan: for debugging
        static int nextnid;
        int nid;  // node id

        static const int NUM_CHILDREN; // the number of partitions
                                       // equals to 4^d, where d is the number of dimensions.

        OptimizedStripesNode<U, K>** childrenPtr;
        bool* isLeaf;

        OptimizedStripesINode* parentPtr;

        // Used to help improving search.
        // A map from  data point to childNodeIndex.
        //static std::tr1::unordered_map<typename K::IDType, ChildNodeIndex* > pointMap[2];
        static std::unordered_map<typename K::IDType, ChildNodeIndex* > pointMap[2];

        // For debugging
        /** Print the pointMap as:
         *  pointid  --->  (nodeid, childIndex)
         */
        void printPointMap(int treeRef) {
//            typename std::tr1::unordered_map<typename K::IDType, ChildNodeIndex*>::const_iterator
//                                            iter = pointMap[treeRef].begin();

            typename std::unordered_map<typename K::IDType, ChildNodeIndex*>::const_iterator
                                            iter = pointMap[treeRef].begin();

            while (iter != pointMap[treeRef].end()) {
                if (iter->second) {
                    /*LOG(DEBUG)*/std::cout << "\t" << iter->first << "  --->  ("
                        << (iter->second)->parent->nid << ", "
                        << (iter->second)->childIndex << ")"
                        << std::endl;
                }

                iter++;

            }
        }

    private:



        /**
         * PD and VD are used to calculate the array index using equation 1.
         * OptimizedStripes: section 4.3
         */
        int PD(double pRef, double pRefQuad, double SL_P) {
            // std::cout << "in PD: " << std::endl
            //           << "\t\tpRef     = " << pRef << std::endl
            //           << "\t\tpRefQuad = " << pRefQuad << std::endl
            //           << "\t\tSL_P     = " << SL_P << std::endl
            //           << "\t\tPD       = " << (((int)ceil((pRef - pRefQuad) / (SL_P / 2))) -1)
            //           << std::endl;

            return (((int)ceil((pRef - pRefQuad) / (SL_P / 2))) -1);
        }

        int VD(double V, double VQuad, double SL_V) {
            // std::cout << "In VD: " << std::endl
            //           << "\t\tV     = " << V << std::endl
            //           << "\t\tVQuad = " << VQuad << std::endl
            //           << "\t\tSL_V     = " << SL_V << std::endl
            //           << "\t\tVD       = " << (((int)ceil((V - VQuad) / (SL_V / 2))) - 1)
            //           << std::endl;

            return (((int)ceil((V - VQuad) / (SL_V / 2))) - 1);
        }
    public:
        static int vxMax;
        static int vyMax;
        static int pxMax;
        static int pyMax;

        static int LIFE_TIME;

        //static int overlapCount;
        //static int disjunctCount;

        OptimizedStripesINode(int level = 0, OptimizedStripesINode* parPtr = NULL) : OptimizedStripesNode<U, K>(level) {
            childrenPtr = new OptimizedStripesNode<U, K>*[NUM_CHILDREN];
            isLeaf = new bool[NUM_CHILDREN];

            // initialize childPtr all to NULL
            // and all pointers point to leaf nodes
            for (int i = 0; i < NUM_CHILDREN; i++) {
                childrenPtr[i] = NULL;
                isLeaf[i] = true;
            }

            parentPtr = parPtr;

            nid = nextnid++;
        }

        ~OptimizedStripesINode() {
            delete [] childrenPtr;
            delete [] isLeaf;
        }

        void insertPoint(U* p, int treeRef, int tickNum , int tickRef) {
            double new0 = K::getKey0(p);
            double new1 = K::getKey1(p);
            double newV0 = K::getV0(p);
            double newV1 = K::getV1(p);

            LOG(DEBUG) << "Tuan: inserting new point: eventually is Point2D("
                << p->id << ", " << new0 << ", " << new1
                << ", " << newV0 << ", " << newV1 << ", " << p->data << ") "
                << std::endl;

            // obtain transformed (V, Pref) tuple
            double transVx = newV0 + vxMax;
            double transVy = newV1 + vyMax;
            double pxRef = new0 - (transVx - vxMax) * (tickNum - tickRef) + vxMax * LIFE_TIME;
            double pyRef = new1 - (transVy - vyMax) * (tickNum - tickRef) + vyMax * LIFE_TIME;

            insertPoint(p, transVx, transVy, pxRef, pyRef, treeRef, tickNum, tickRef);
        }

        void insertPoint(U* p, double transVx, double transVy, double pxRef, double pyRef, int treeRef, int tickNum , int tickRef) {

            LOG(DEBUG) << "Tuan: transformed (V, Pref) tuple:" << std::endl
                << "\t(transVx, pxRef) = (" << transVx << ", " << pxRef << ")" << std::endl
                << "\t(transVy, pyRef) = (" << transVy << ", " << pyRef << ")" << std::endl;


            // if(tickNum == 0) {
            //     std::cout << "Tuan: inserting new point " << *p << " into internal node: " << *this
            //               << " of tree " << treeRef << std::endl;
            // }
            // increment the size of the node
            OptimizedStripesNode<U, K>::size++;

            // calculate array index in the children pointer array
            // we hardcode d=2 in this case
            // TODO: use the NUM_DIM parameter instead

            // std::cout << "pxMax: " << pxMax << std::endl;
            // std::cout << "pyMax: " << pyMax << std::endl;
            // std::cout << "vxMax: " << pxMax << std::endl;
            // std::cout << "vyMax: " << pyMax << std::endl;
            // std::cout << "LIFE_TIME: " << LIFE_TIME << std::endl;

            double SL_Px = (double)(pxMax + 2 * vxMax * LIFE_TIME) / (1 << OptimizedStripesNode<U, K>::level);
            double SL_Py = (double)(pyMax + 2 * vyMax * LIFE_TIME) / (1 << OptimizedStripesNode<U, K>::level);

            double SL_Vx = (double)(2 * vxMax) / (1 << OptimizedStripesNode<U, K>::level);
            double SL_Vy = (double)(2 * vyMax) / (1 << OptimizedStripesNode<U, K>::level);

            // if(tickNum == 0) {
                // std::cout << "\tlevel      = " << OptimizedStripesNode<U, K>::level << std::endl
                //           << "\tpxRef      = " << pxRef << std::endl
                //           << "\tpyRef      = " << pyRef << std::endl
                //           << "\tSL_Py      = " << SL_Py << std::endl
                //           << "\tSL_Px      = " << SL_Px << std::endl
                //           << "\tSL_Py      = " << SL_Py << std::endl
                //           << "\tSL_Vx      = " << SL_Vx << std::endl
                //           << "\tSL_Vy      = " << SL_Vy << std::endl
                //           << "\tgrid       = (" << OptimizedStripesNode<U, K>::grid.vx
                //           << ", " << OptimizedStripesNode<U, K>::grid.px
                //           << ", " << OptimizedStripesNode<U, K>::grid.vy
                //           << ", " << OptimizedStripesNode<U, K>::grid.py
                //           << ")" << std::endl;
            // }

            DBUTL_ASSERT((transVx >= OptimizedStripesNode<U, K>::grid.vx));
            DBUTL_ASSERT((transVy >= OptimizedStripesNode<U, K>::grid.vy));
            DBUTL_ASSERT((pxRef >= OptimizedStripesNode<U, K>::grid.px));
            DBUTL_ASSERT((pyRef >= OptimizedStripesNode<U, K>::grid.py));




            int PD3 = PD(pxRef, OptimizedStripesNode<U, K>::grid.px, SL_Px);
            int VD2 = VD(transVx, OptimizedStripesNode<U, K>::grid.vx, SL_Vx);
            int PD1 = PD(pyRef, OptimizedStripesNode<U, K>::grid.py, SL_Py);
            int VD0 = VD(transVy, OptimizedStripesNode<U, K>::grid.vy, SL_Vy);

            int index = 8 * PD3 + 4 * VD2 + 2 * PD1 + VD0;



            // if(tickNum == 0) {
            // std::cout << "\tPD3 = " << PD3 << std::endl
            //           << "\tVD2 = " << VD2 << std::endl
            //           << "\tPD1 = " << PD1 << std::endl
            //           << "\tVD0 = " << VD0 << std::endl;
            
            // std::cout << "Tuan: insertPoint: index is: " << index << std::endl;
            // }


            if (isLeaf[index]) { // the child pointer points to a leaf node

                LOG(DEBUG) << "\tThe child pointer points to a leaf node." << std::endl;

                if (childrenPtr[index] == NULL) { // case 1: the target leaf node is non-existent.

                    LOG(DEBUG) << "\tThe target leaf node is non-existent." << std::endl;

                    OptimizedStripesNode<U, K>* leafNode 
                        = new OptimizedStripesLNode<U, K>(OptimizedStripesNode<U, K>::level + 1);

                    // TODO: should the new entry be the vectors (V,Pref)
                    dynamic_cast<OptimizedStripesLNode<U, K>* >(leafNode)->insertPoint(p, transVx, transVy, pxRef, pyRef);
                    childrenPtr[index] = leafNode;

                    ChildNodeIndex* childNodeIndex = new ChildNodeIndex(this, index);

                    LOG(DEBUG) << "\tInsert to the pointMap " << treeRef << " : "
                               << K::getOID(p) << "  --->  ("
                               << childNodeIndex->parent->nid << ", "
                               << childNodeIndex->childIndex << ")"
                               << std::endl;

                    pointMap[treeRef][K::getOID(p)] = childNodeIndex;
                    

                    //leafNode->level++;
                    //leafNode->setGrid(newV0, new0, newV1, new1);

                    LOG(DEBUG) << "\tThe new leaf node " << *(dynamic_cast<OptimizedStripesLNode<U, K>* >(leafNode)) << " was created and the point was inserted." << std::endl;
                } else {
                    DBUTL_ASSERT((dynamic_cast<OptimizedStripesLNode<U, K>*>(childrenPtr[index])));

                    OptimizedStripesLNode<U, K>* leafNode = dynamic_cast<OptimizedStripesLNode<U, K>*>(childrenPtr[index]);
                    if (leafNode->isFull()) { // case 3: the target leaf node is found and is full.

                        LOG(DEBUG) << "\tThe target leaf node is found and is full." << std::endl;

                        OptimizedStripesINode<U, K>* internalNode = new OptimizedStripesINode<U, K>(OptimizedStripesNode<U, K>::level + 1, this);

                        internalNode->setGrid(OptimizedStripesNode<U, K>::grid.vx + (double)VD2 * SL_Vx / 2,
                                              OptimizedStripesNode<U, K>::grid.px + (double)PD3 * SL_Px / 2,
                                              OptimizedStripesNode<U, K>::grid.vy + (double)VD0 * SL_Vy / 2,
                                              OptimizedStripesNode<U, K>::grid.py + (double)PD1 * SL_Py / 2);

                        LOG(DEBUG) << "\tA new internal was created: " << *internalNode << std::endl;

                        LOG(DEBUG) << "Tuan: distributing old entries to the new node" << std::endl;
                        for (int i = 0; i < leafNode->size; i++) {
                            U* point = leafNode->entries[i];
                            internalNode->insertPoint(leafNode->entries[i],
                                                      leafNode->transEntries[i]->transVx,
                                                      leafNode->transEntries[i]->transVy,
                                                      leafNode->transEntries[i]->pxRef,
                                                      leafNode->transEntries[i]->pyRef,
                                                      treeRef, tickNum, tickRef);
                        }

                        internalNode->insertPoint(p, transVx, transVy, pxRef, pyRef,
                                                  treeRef, tickNum, tickRef);


                        childrenPtr[index] = internalNode;
                        isLeaf[index] = false;


                        delete leafNode;

                    } else { // case 2: the target leaf node is found and is not full.
                        dynamic_cast<OptimizedStripesLNode<U, K>* >(leafNode)->insertPoint(p, transVx, transVy, pxRef, pyRef);
                        ChildNodeIndex* childNodeIndex = new ChildNodeIndex(this, index);
                        pointMap[treeRef][K::getOID(p)] = childNodeIndex;
                    }
                }
            } else {
                dynamic_cast<OptimizedStripesINode<U, K>* >(
                    childrenPtr[index])->insertPoint(p, 
                                                     transVx, 
                                                     transVy, 
                                                     pxRef, 
                                                     pyRef, 
                                                     treeRef, 
                                                     tickNum, 
                                                     tickRef);
            }

        }

        void collectEntries(std::vector<U*>* collectedEntries) {
            for (int i = 0; i < NUM_CHILDREN; i++) {
                if (childrenPtr[i] != NULL) {
                    if (isLeaf[i]) {
                        for (int k = 0; k < childrenPtr[i]->size; k++) {
                            collectedEntries->push_back((dynamic_cast<OptimizedStripesLNode<U, K>* >(childrenPtr[i]))->entries[k]);
                        }
                        // delete the leaf node
                        delete childrenPtr[i];
                    } else {
                        // This branch is for sanity check.
                        // If the node is under-filled then all its children are leaf nodes.
                        (dynamic_cast<OptimizedStripesINode<U, K>* >(childrenPtr[i]))->collectEntries(collectedEntries);
                        // delete the internal node
                        delete childrenPtr[i];
                    }
                }
            }
        }

        /**
         * This function is called from deletePoint(oid, treeRef) of an internal node.
         */
        bool deletePoint(typename K::IDType oid, int childIndex, int treeRef) {


            // std::cout << "childIndex=" << childIndex
            //           << " size=" << OptimizedStripesNode<U, K>::size
            //           << " nid=" << nid
            //           << " treeRef=" << treeRef 
            //           << " oid=" << oid
            //           << std::endl;


            // printPointMap(treeRef);

            if(OptimizedStripesNode<U, K>::size == 1 && childIndex == 1) {
                return 0;
            }


            if (childrenPtr[childIndex] == NULL) {
                LOG(DEBUG) << "Tuan: the point is not there." << std::endl;
                return false;
            }

            DBUTL_ASSERT(isLeaf[childIndex]);

            OptimizedStripesLNode<U, K>* leaf = dynamic_cast<OptimizedStripesLNode<U, K>*>(childrenPtr[childIndex]);

            bool ans = leaf->deletePoint(oid);

            if (ans == false) return false;

            // should we use erase function?
            pointMap[treeRef][oid] = NULL;

            if (leaf->isEmpty()) {
                LOG(DEBUG) << "Tuan: the leaf node is empty, delete the leaf node." << std::endl;
                delete leaf;
                childrenPtr[childIndex] = NULL;
            }

            // Adjust the size of internal nodes
            OptimizedStripesNode<U, K>::size--;
            OptimizedStripesINode<U, K>* parent = parentPtr;
            while (parent != NULL) {
                parent->size--;
                parent = parent->parentPtr;
            }

            // TODO: check for under-filled
           // if (OptimizedStripesNode<U, K>::size < OptimizedStripesLNode<U, K>::MAXENTRIES) { // the node is under-filled
           if (false) {
                LOG(DEBUG) << "Tuan: the node is under-filled" << std::endl;
                if (parentPtr == NULL) {
                    LOG(DEBUG) << "Tuan: reach the root node, just return." << std::endl;
                    return  ans;
                } else { // collect all the entries and put them to a new leaf node
                    LOG(DEBUG) << "\tCollect all entries and put them into a new leaf node." << std::endl;
                    OptimizedStripesLNode<U, K>* leafNode = new OptimizedStripesLNode<U, K>();

                    std::vector<U*> collectedEntries;
                    collectEntries(&collectedEntries);

                    DBUTL_ASSERT((collectedEntries.size() < OptimizedStripesLNode<U, K>::MAXENTRIES));

                    for (int i = 0; i < collectedEntries.size(); i++) {
                        // TODO: fix insertPoint
                        //leafNode->insertPoint(collectedEntries[i]);
                    }

                    // get the first point, from the hash, we determine the index of this node.
                    // the assumption is that: if a node is under-filled, all its children are leaf nodes.
                    ChildNodeIndex* childNodeIndex = pointMap[treeRef][K::getOID(collectedEntries[0])];
                    int index = childNodeIndex->childIndex;

                    if (collectedEntries.size() > 0) {
                        LOG(DEBUG) << "\tThe number of collected entries is: " << collectedEntries.size() << std::endl;
                        parentPtr->childrenPtr[index] = leafNode;
                    } else {
                        LOG(DEBUG) << "\tNo entries have been collected." << std::endl;
                        parentPtr->childrenPtr[index] = NULL;
                    }
                    parentPtr->isLeaf[index] = true;
                }
            }

           return ans;
        }

        /* *
         * Funtion returns true if point is found (then the index is deleted)
         * other return false
         */
        bool deletePoint(typename K::IDType oid, int treeRef) {
            LOG(DEBUG) << "Tuan: deletePoint: " << oid << std::endl;

            ChildNodeIndex* childNodeIndex = pointMap[treeRef][oid];

            if (childNodeIndex == NULL) {
                LOG(DEBUG) << "Tuan: the point was not found in tree " << treeRef << std::endl;
                return false;
            }

            OptimizedStripesINode<U, K>* node = childNodeIndex->parent;
            LOG(DEBUG) << "Tuan: the point is found at internal node: "
                       << node->nid << "\t of tree " << treeRef << std::endl;

            return node->deletePoint(oid, childNodeIndex->childIndex, treeRef);
        }

        static const int INSIDE = 0;
        static const int DISJUNCT = 1;
        static const int OVERLAP = 2;


        double getPref(double p, double V, double vMax, int tickNum, int tickRef) {
            if ( floor(p) == 31 && floor(V) == 1007 ) {
                LOG(DEBUG) << "Tuan: getRef calculation.........." << std::endl
                    << "\t\t\t p = " << p << std::endl
                    << "\t\t\t V = " << V << std::endl
                    << "\t\t\t vMax = " << vMax << std::endl
                    << "\t\t\t tickNum = " << tickNum << std::endl
                    << "\t\t\t tickRef = " << tickRef << std::endl;

                LOG(DEBUG) << "Tuan: recalculate point 24......." << std::endl
                    << "\ttransVx = " << -5 + 1000
                    << "\ttransVy = " <<  8 + 1000
                    << "\tpxRef   = " << 31 - (-5) * (tickNum - tickRef) + 1000 * 2
                    << "\tpyRef   = " << 35 - (8)  * (tickNum - tickRef) + 1000 * 2
                    << std::endl;
            }

            return  p - (V - vMax) * (tickNum - tickRef) + vMax * LIFE_TIME;

            //if ((tickNum % LIFE_TIME) == 0) {
            //   return  p - (V - vMax) * (tickNum - tickRef) + vMax * LIFE_TIME;
            //} else {
            //   return  p - (V - vMax) * (tickNum - tickRef - 1) + vMax * LIFE_TIME;
            //}
            //return  p - (V - vMax) + vMax * LIFE_TIME;
            //return  p + vMax * LIFE_TIME;
            //return  p + V * LIFE_TIME;
        }

        double getPref(double p, double v) {
            return p + v * LIFE_TIME;
        }
        bool isBelow(double ur_v, double ur_p, double pl, double vMax, int tickNum, int tickRef) {

           double Pref = getPref(pl, ur_v, vMax, tickNum, tickRef);

            if (Pref > ur_p) {
                LOG(DEBUG) << "Tuan: isBelow: " << std::endl
                    << "\t\t  tickNum = " << tickNum << std::endl
                    << "\t\t  tickRef = " << tickRef << std::endl
                    << "\t\t  ur_v = " << ur_v << std::endl
                    << "\t\t  ur_p = " << ur_p << std::endl
                    << "\t\t  pl = " << pl << std::endl
                    << "\t\t  Pref = " << Pref << std::endl;

                return true;
            } else {
                return false;
            }
        }
        bool isAbove(double ll_v, double ll_p, double pu, double vMax, int tickNum, int tickRef ) {

            double Pref = getPref(pu, ll_v, vMax, tickNum, tickRef);

            if (Pref < ll_p) {
                LOG(DEBUG) << "isAbove: "
                    << "\t\t" << "Pref = " << Pref << "\t\tll_p = " << ll_p
                    << "\t\t" << "pu = " << pu << "\t\tll_v = " << ll_v << std::endl
                    << "\t\t" << "tickNum = " << tickNum << "\t\ttickRef = " << tickRef
                    << std::endl;
                return true;
            } else {
                return false;
            }
        }
        bool isOnOrBelow(double ur_v, double ur_p, double pu, double vMax, int tickNum, int tickRef) {
            double Pref = getPref(pu, ur_v, vMax, tickNum, tickRef);

            if (Pref >= ur_p) {
                return true;
            } else {
                return false;
            }
        }
        bool isOnOrAbove(double ll_v, double ll_p, double pl, double vMax, int tickNum, int tickRef) {

            double Pref = getPref(pl, ll_v, vMax, tickNum, tickRef);
            
            //std::cout << "Pref: " << Pref << " ll_p: " << ll_p << std::endl;

            if (Pref <= ll_p) {
                return true;
            } else {
                return false;
            }
        }
        int getRelativePosition(RType* r, int tickNum, int tickRef) {
            LOG(DEBUG) << "Tuan: querying node: " << *this << std::endl;

            //LOG(DEBUG) << "Tuan: getting the relative position:" << std::endl;
            double pl_x = r->getLow0();
            double pu_x = r->getHigh0();

            double ll_vx = OptimizedStripesNode<U, K>::grid.vx;
            double ll_px = OptimizedStripesNode<U, K>::grid.px;

            double SL_Px = (double)(pxMax + 2 * vxMax * LIFE_TIME) / (1 << OptimizedStripesNode<U, K>::level);

            double SL_Vx = (double)(2 * vxMax) / (1 << OptimizedStripesNode<U, K>::level);

            double ur_vx = ll_vx + SL_Vx;
            double ur_px = ll_px + SL_Px;


            // std::cout << "testing rel pos on the X plane: " << std::endl;
            // std::cout << "\t\tpl_x = " << pl_x << std::endl
            //            << "\t\tpu_x = " << pu_x << std::endl
            //            << "\t\tSL_Vx = " << SL_Vx << std::endl
            //            << "\t\tSL_Px = " << SL_Px << std::endl
            //            << "\t\tll_vx = " << ll_vx << std::endl
            //            << "\t\tll_px = " << ll_px << std::endl
            //            << "\t\tur_vx = " << ur_vx << std::endl
            //            << "\t\tur_px = " << ur_px << std::endl;


            int relPosX;

            if (isOnOrAbove(ll_vx, ll_px, pl_x, vxMax, tickNum, tickRef) &&
                isOnOrBelow(ur_vx, ur_px, pu_x, vxMax, tickNum, tickRef)) {
                //std::cout << "\t On the X plane: INSIDE." << std::endl;
                relPosX = INSIDE;
            } else if (isAbove(ll_vx, ll_px, pu_x, vxMax, tickNum, tickRef) ||
                      isBelow(ur_vx, ur_px, pl_x, vxMax, tickNum, tickRef)) {
                //std::cout << "\t On the X plane: DISJUNCT." << std::endl;
                relPosX = DISJUNCT;
            } else {
                //std::cout << "\t On the X plane: OVERLAP." << std::endl;
                relPosX = OVERLAP;
            }

            double pl_y = r->getLow1();
            double pu_y = r->getHigh1();
            double ll_vy = OptimizedStripesNode<U, K>::grid.vy;
            double ll_py = OptimizedStripesNode<U, K>::grid.py;

            double SL_Py = (double)(pyMax + 2 * vyMax * LIFE_TIME) / (1 << OptimizedStripesNode<U, K>::level);

            double SL_Vy = (double)(2 * vyMax) / (1 << OptimizedStripesNode<U, K>::level);

            double ur_vy = ll_vy + SL_Vy;
            double ur_py = ll_py + SL_Py;

            
            // std::cout << "testing rel pos on the Y plane: " << std::endl
            //           << "\t\tpl_y = " << pl_y << std::endl
            //           << "\t\tpu_y = " << pu_y << std::endl
            //           << "\t\tSL_Vy = " << SL_Vy << std::endl
            //           << "\t\tSL_Py = " << SL_Py << std::endl
            //           << "\t\tll_vy = " << ll_vy << std::endl
            //           << "\t\tll_py = " << ll_py << std::endl
            //           << "\t\tur_vy = " << ur_vy << std::endl
            //           << "\t\tur_py = " << ur_py << std::endl;




            int relPosY;
            if (isOnOrAbove(ll_vy, ll_py, pl_y, vyMax, tickNum, tickRef) &&
                isOnOrBelow(ur_vy, ur_py, pu_y, vyMax, tickNum, tickRef)) {
                //std::cout << "\t On the Y plane: INSIDE." << std::endl;
                relPosY = INSIDE;
            } else if (isAbove(ll_vy, ll_py, pu_y, vyMax, tickNum, tickRef) ||
                       isBelow(ur_vy, ur_py, pl_y, vyMax, tickNum, tickRef)) {
                //std::cout << "\t On the Y plane: DISJUNCT." << std::endl;


                relPosY = DISJUNCT;
            } else {
                //std::cout << "\t On the Y plane: OVERLAP." << std::endl;
                relPosY = OVERLAP;
            }
            if ((relPosX == INSIDE) && (relPosY == INSIDE)) {
                return INSIDE;
            } else if ((relPosX == DISJUNCT) || (relPosY == DISJUNCT)) {
                return DISJUNCT;
            } else {
                return OVERLAP;
            }
        }
        // r is tmp for debugging. 
        void reportAll(IndexCallBack<U>* cb, RType *r) {

            OptimizedStripesNode<U, K>::numReport++;            

            for (int i = 0; i < OptimizedStripesINode<U, K>::NUM_CHILDREN; i++) {
                if (childrenPtr[i] != NULL) {
                    if (isLeaf[i]) {
                        OptimizedStripesLNode<U, K>* leaf = dynamic_cast<OptimizedStripesLNode<U, K>*>(childrenPtr[i]);
                        for (int j = 0; j < leaf->size; j++) {
                            LOG(DEBUG) << "Tuan: reporting point: " << *(leaf->entries[j]) << std::endl;

                            U* point  = leaf->entries[j];
                            if (r->template containsPoint<U,K>(point)) { //tmp: remove when done.
                                cb->reportPoint(leaf->entries[j]);
                            }
#ifdef DEBUG_STRIPES
                            OptimizedStripesNode<U, K>::tuanTBD.push_back(leaf->entries[j]);
#endif
                        }
                    } else {
                        OptimizedStripesINode<U, K>* inode = dynamic_cast<OptimizedStripesINode<U, K>*>(childrenPtr[i]);
                        inode->reportAll(cb, r);
                    }
                }
            }

        }
        //CURRENT

        void printQuads(FILE* gp, bool xDim) {
            double SL_Px, SL_Vx;
            double SL_Py, SL_Vy;

            if(xDim) {
                SL_Px = (double)(pxMax + 2 * vxMax * LIFE_TIME) 
                    / (1 << OptimizedStripesNode<U, K>::level);

                SL_Vx = (double)(2 * vxMax) 
                    / (1 << OptimizedStripesNode<U, K>::level);

                double ll_px = OptimizedStripesNode<U, K>::grid.px;
                double ll_vx = OptimizedStripesNode<U, K>::grid.vx;

                double ur_px = ll_px + SL_Px;
                double ur_vx = ll_vx + SL_Vx;


                //fprintf(gp, "%f %f %f %f\n", ll_vx, ll_px, ur_vx, ur_px);
                fprintf(gp, "set obj %d rect from %d,%d to %d, %d\n", 
                        (nid+1), (int)ll_vx, (int)ll_px, (int)ur_vx, (int)ur_px);


                //std::cout << ll_vx  << " " <<  ll_px << " " << ur_vx << " " << ur_px << std::endl;
            }
            else {
                SL_Py = (double)(pyMax + 2 * vyMax * LIFE_TIME) 
                    / (1 << OptimizedStripesNode<U, K>::level);
                SL_Vy = (double)(2 * vyMax) 
                    / (1 << OptimizedStripesNode<U, K>::level);


                double ll_py = OptimizedStripesNode<U, K>::grid.py;
                double ll_vy = OptimizedStripesNode<U, K>::grid.vy;

                double ur_py = ll_py + SL_Py;
                double ur_vy = ll_vy + SL_Vy;

                //fprintf(gp, "%f %f %f %f\n", ll_vy, ll_py, ur_vy, ur_py);
                fprintf(gp, "set obj %d rect from %d,%d to %d, %d\n", 
                        (nid + 1), (int)ll_vy, (int)ll_py, (int)ur_vy, (int)ur_py);


                //std::cout << ll_vy  << " " <<  ll_py << " " << ur_vy << " " << ur_py << std::endl;
            }

            for (int i = 0; i < NUM_CHILDREN; i++) {
                if (childrenPtr[i] != NULL) {
                    if (isLeaf[i]) {
                         OptimizedStripesLNode<U, K>* leaf 
                             = dynamic_cast<OptimizedStripesLNode<U, K>*>(childrenPtr[i]);


                         double SL_P = (double)(pyMax + 2 * vyMax * LIFE_TIME) 
                             / (1 << (OptimizedStripesNode<U, K>::level + 1));
                         double SL_V = (double)(2 * vyMax) 
                             / (1 << (OptimizedStripesNode<U, K>::level + 1));

                         leaf->printQuads(gp, xDim, SL_P, SL_V);
                    } else {

                        OptimizedStripesINode<U, K>* inode = dynamic_cast<OptimizedStripesINode<U, K>*>(childrenPtr[i]);
                        inode->printQuads(gp, xDim); 
                    }
                }
            }
        }


        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r, int tickNum, int tickRef) {

#ifdef DEBUG_COUNT
            OptimizedStripesNode<U, K>::numInternalVisits++;
            //std::cerr << *this << std::endl;
#endif



            // if(levelMap.count(OptimizedStripesNode<U, K>::level) == 0) {
            //     levelMap[OptimizedStripesNode<U, K>::level] = 0;
            // }
            // levelMap[OptimizedStripesNode<U, K>::level]++;


            // std::cout << "visiting internal node " << *this << std::endl;
            // std::cout << "Tuan: enumerating over the region: " << *r << std::endl;

            int relativePosition = getRelativePosition(r, tickNum, tickRef);

            //if (relativePosition == INSIDE) {
                //std::cout << "Tuan: the node lies INSIDE query region, report all the entries." << std::endl;
            //reportAll(cb, r);
            /*} else*/ if (relativePosition == DISJUNCT) {
                //disjunctCount++;
                // std::cout << "Tuan: the node is DISJUNCT with query region, ignore all the entries." << std::endl;
            } else {
                //overlapCount++;
                // std::cout << "Tuan: the node is OVERLAP with query region, probe recusively " << std::endl;
                for (int i = 0; i < NUM_CHILDREN; i++) {
                    if (childrenPtr[i] != NULL) {
                        if (isLeaf[i]) {
                            OptimizedStripesLNode<U, K>* leaf = dynamic_cast<OptimizedStripesLNode<U, K>*>(childrenPtr[i]);
                            leaf->enumerateOverRegion(cb, r);
                        } else {
                            OptimizedStripesINode<U, K>* inode = dynamic_cast<OptimizedStripesINode<U, K>*>(childrenPtr[i]);
                            inode->enumerateOverRegion(cb, r, tickNum, tickRef);
                        }
                    }
                }
            }
        }

        // void autoUpdate() {
        //     std::vector<U*> unupdatedPoints;
        //     deleteUnupdatedPoints(&unupdatedPoints);

        //     // Tuan: for debugging
        //     LOG(DEBUG) << "Tuan: un-updated points are: " << std::endl;

        //     for (int i = 0; i < unupdatedPoints.size(); i++) {
        //         LOG(DEBUG) << "\t\t" << *(unupdatedPoints[i]) << std::endl;
        //     }

        //     LOG(DEBUG) << "Tuan: re-insert unupdated points:" << std::endl;
        // }

        int deleteUnupdatedPoints(std::vector<U*>* unupdatedPoints, int treeRef) {
            int numDeleted = 0;
            
            for (int i = 0; i < NUM_CHILDREN; i++) {
                if (childrenPtr[i] != NULL) {
                    if (isLeaf[i]) {
                        OptimizedStripesLNode<U, K>* leaf
                            = dynamic_cast<OptimizedStripesLNode<U, K>*>(childrenPtr[i]);

                        numDeleted += leaf->size;
                        
                        for (int j = 0; j < leaf->size; j++) {
                            LOG(DEBUG) << "Tuan: gathering point: " 
                                       << *(leaf->entries[j]) << std::endl;

                            OptimizedStripesINode<U, K>::pointMap[treeRef][K::getOID(leaf->entries[j])] = NULL;

                            unupdatedPoints->push_back(leaf->entries[j]);
                        }
                        
                        delete leaf;
                    } else {
                        OptimizedStripesINode<U, K>* inode
                            = dynamic_cast<OptimizedStripesINode<U, K>*>(childrenPtr[i]);
                        numDeleted += inode->deleteUnupdatedPoints(unupdatedPoints, treeRef);
                        isLeaf[i] = true;
                        delete inode;
                    }
                    childrenPtr[i] = NULL;
                }
            }
            return numDeleted;
        }

        /**
         * Prints a node to out stream using the format
         *    OptimizedStripesINode(id, level, {grid},size)
         *    where {grid} is displayed as {(vx,px),(vy,py)}
         */
        friend std::ostream& operator<<(std::ostream &out, const OptimizedStripesINode<U, K>& node) {
            //std::cout << "in print internal node" << std::endl;
            //out << "OptimizedStripesINode";

            out << "OptimizedStripesINode(" << std::setiosflags(std::ios::fixed) << std::setprecision(2)
                << node.nid << ", " << node.level << ", "
                << "{(" << node.grid.vx << "," << node.grid.px << "), (" << node.grid.vy << "," << node.grid.py << ")}"
                << ", " << node.size << ")";

        }

    };
    template <typename U, typename K>
    const int OptimizedStripesINode<U, K>::NUM_CHILDREN = 16;
    template <typename U, typename K>
    int OptimizedStripesINode<U, K>::vxMax = std::numeric_limits<int>::max();
    template <typename U, typename K>
    int OptimizedStripesINode<U, K>::vyMax = std::numeric_limits<int>::max();
    template <typename U, typename K>
    int OptimizedStripesINode<U, K>::pxMax = std::numeric_limits<int>::max();
    template <typename U, typename K>
    int OptimizedStripesINode<U, K>::pyMax = std::numeric_limits<int>::max();
    template <typename U, typename K>
    int OptimizedStripesINode<U, K>::LIFE_TIME = 2;
    // template <typename U, typename K>
    // int OptimizedStripesINode<U, K>::overlapCount = 0;
    // template <typename U, typename K>
    // int OptimizedStripesINode<U, K>::disjunctCount = 0;
    template <typename U, typename K>
    int OptimizedStripesINode<U, K>::nextnid = 0;
//    template <typename U, typename K>
//    std::tr1::unordered_map<typename K::IDType, U*> OptimizedStripesINode<U, K>::velocityUpdatedPointMap;
    template <typename U, typename K>
//    std::tr1::unordered_map<typename K::IDType, typename OptimizedStripesINode<U,K>::ChildNodeIndex* >
//                OptimizedStripesINode<U, K>::pointMap[2];
    std::unordered_map<typename K::IDType, typename OptimizedStripesINode<U,K>::ChildNodeIndex* >
                    OptimizedStripesINode<U, K>::pointMap[2];


    template <typename U, typename K, typename A>
    class OptimizedStripes : public MovingSpatialIndex2D<U, K, A> {
    private:
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        bool transitionTick;
        int idx0;
        int idx1;

        int lifeTime;


        int localVXMax, localVYMax;


        char name[100];

        void printQuads(int index, bool xDim, std::vector<U*>* agents, const char* name) {

            //std::cout << "Printing quads: " << std::endl;

            FILE* gp = popen("gnuplot", "w"); 

            fprintf(gp, "set terminal postscript eps enhanced color;\n");
            fprintf(gp, "set output '%s';\n", name);
            fprintf(gp, "unset key;\n");
            //fprintf(gp, "set view map;");
            //fprintf(gp, "set style data lines;");
            //fprintf(gp, "set title 'Point Heat Map';");
            //fprintf(gp, "set cblabel 'Score';");
            //fprintf(gp, "set palette rgbformulae -7, 2, -7;");



            double vxMax = OptimizedStripesINode<U, K>::vxMax;
            double pxMax = OptimizedStripesINode<U, K>::pxMax;

            double vyMax = OptimizedStripesINode<U, K>::vyMax;
            double pyMax = OptimizedStripesINode<U, K>::pyMax;

            int lt = OptimizedStripesINode<U, K>::LIFE_TIME;

            double space_Px = (double)(pxMax + 2 * vxMax * lt);
            double space_Vx = (double)(2 * vxMax);

            double space_Py = (double)(pyMax + 2 * vyMax * lt) / 1;
            double space_Vy = (double)(2 * vyMax) / 1;
            
            //fprintf(gp, "plot '-' using 1:2:($3-$1):($4-$2) notitle with vectors\n");


            
            tree[index]->printQuads(gp, xDim);
            

            int treeTickRef = tickRef[index];


            if(xDim) {
                //fprintf(gp, "plot [0:%d][0:%d] '-' using 1:2 notitle lc rgb \"green\"\n", 
                fprintf(gp, "plot [9500:10500][18000:32000] '-' using 1:2 notitle lc rgb \"green\"\n");

            }
            else {
                fprintf(gp, "plot [0:%d][0:%d] '-' using 3:4 notitle lc rgb \"green\"\n", 
                        (int)space_Vy, (int)space_Py);
            }


            for(int i = 0; i < agents->size(); i++) {
                U* u = agents->at(i);

                double transVx = K::getV0(u) + vxMax;
                double transVy = K::getV1(u) + vyMax;
                double pxRef = K::getKey0(u) 
                    - (transVx - vxMax) * (tickNum - treeTickRef) + vxMax * lt;

                double pyRef = K::getKey1(u) 
                    - (transVy - vyMax) * (tickNum - treeTickRef) + vyMax * lt;

                fprintf(gp, "%lf %lf %lf %lf\n", transVx, pxRef, transVy, pxRef);
            }

            fprintf(gp,"e\n");
            pclose(gp);
        }



        // // Tuan: for debugging
        // void setup() {
        //     //OptimizedStripesINode<U, K>::vxMax = 20;
        //     OptimizedStripesINode<U, K>::vxMax = 1000;
        //     //OptimizedStripesINode<U, K>::vyMax = 20;
        //     OptimizedStripesINode<U, K>::vyMax = 1000;
        //     //OptimizedStripesINode<U, K>::pxMax = 100;
        //     OptimizedStripesINode<U, K>::pxMax = 100000;
        //     //OptimizedStripesINode<U, K>::pyMax = 100;
        //     OptimizedStripesINode<U, K>::pyMax = 100000;
        //}
    public:
        OptimizedStripes(int vxMax = 200, int vyMax = 200,
                         int pxMax = 10000, int pyMax = 10000,
                         int life_time = 20) {
            std::cout << "vxMax: " << vxMax << std::endl;
            std::cout << "vyMax: " << vyMax << std::endl;


            localVXMax = vxMax;
            localVYMax = vyMax;

            OptimizedStripesINode<U, K>::vxMax = vxMax;
            OptimizedStripesINode<U, K>::vyMax = vyMax;
            OptimizedStripesINode<U, K>::pxMax = pxMax;
            OptimizedStripesINode<U, K>::pyMax = pyMax;

            sprintf(name, "OptimizedStripes%d", life_time);


            // OptimizedStripesINode<U, K>::vxMax = 1000000;
            // OptimizedStripesINode<U, K>::vyMax = 1000000;
            // OptimizedStripesINode<U, K>::pxMax = 1000000;
            // OptimizedStripesINode<U, K>::pyMax = 1000000;


            lifeTime = life_time;

            OptimizedStripesINode<U, K>::LIFE_TIME = life_time;

            tickNum = 0;
            idx0 = 0;
            idx1 = 1;

            tickRef[idx0] = 0;
            //tickRef[idx1] = OptimizedStripesINode<U, K>::LIFE_TIME;
            tickRef[idx1] = 0;


            transitionTick = false;

            // Tuan: for debugging
            //setup();

/*             std::cout << "Tuan: the parameters are:" << std::endl */
/*                       << "\t\tvxMax = " << OptimizedStripesINode<U, K>::vxMax << std::endl */
/*                       << "\t\tvyMax = " << OptimizedStripesINode<U, K>::vyMax << std::endl */
/*                       << "\t\tpxMax = " << OptimizedStripesINode<U, K>::pxMax << std::endl */
/*                       << "\t\tpyMax = " << OptimizedStripesINode<U, K>::pyMax << std::endl */
/*                       << "\t\tL     = " << OptimizedStripesINode<U, K>::LIFE_TIME << std::endl; */

        }
        ~OptimizedStripes() {
#ifdef DEBUG_COUNT

            
            for (int i = 0; i < tree0_size.size(); i++) {
                std::cerr << i << "\t" << tree0_size[i] << "\t" << tree1_size[i] << "\t" << numInternalNodeVisits[i] << "\t" << numLeafNodeVisits[i] << "\t" << numReportAll[i] << std::endl;
            }
#endif

            // std::map<int, uint32_t>::iterator it;

            // for(it = levelMap.begin(); it != levelMap.end(); it++) {
            //     std::cout << "level " << it->first << ": " << it->second << std::endl;   
            // }


            //TODO: delete the whole tree.
            delete tree;
        }

        static const char* getClassName() { return "OptimizedStripes"; }
        const char* getInstanceName() { return name; } //getClassName(); }

        void startTick(std::vector<U*>* agents) {
            // std::cout << "Starting Tick " << tickNum << std::endl;
            // std::cout << "transition tick: " << transitionTick << std::endl;
            
            // Tuan: Debugging
            // LOG(DEBUG) << "Tuan: the points at the beginning are:" << std::endl;

            // for (int i = 0; i < (*agents).size(); i++) {
            //     LOG(DEBUG) << "\t\t" << *(*agents)[i] << std::endl;
            // }






            if (tickNum == 0) { // the very first tick

                // Hack to get maxVx/maxVy set appropriately for debugging...
                OptimizedStripesINode<U, K>::vxMax = localVXMax;
                OptimizedStripesINode<U, K>::vyMax = localVYMax;


                OptimizedStripesINode<U, K>::LIFE_TIME = lifeTime;

                tree = new OptimizedStripesINode<U, K>*[NUM_DIM];
                for (int i = 0; i < NUM_DIM; i++) {
                    tree[i] = new OptimizedStripesINode<U, K>();
                }

                LOG(DEBUG) << "Tuan: build the first tree at the very first tick." << std::endl;
                for (int i = 0; i < (*agents).size(); i++) {
                    LOG(DEBUG) << "Tuan: startTick: insert point: " << *(*agents)[i] << std::endl;
                    U* point = (*agents)[i];
                    tree[idx1]->insertPoint(point, idx1, tickNum, tickRef[idx1]);
                }
            }




#ifdef DEBUG_COUNT

            // std::cout << "idx0 = " << idx0 << std::endl;
            // std::cout << "idx1 = " << idx1 << std::endl;

            // std::cout << "Tree idx0 size: " << tree[idx0]->size << std::endl;
            // std::cout << "Tree idx1 size: " << tree[idx1]->size << std::endl;

            // std::cout << "Tree idx0 height: " << computeHeight(tree[idx0]) << std::endl;
            // std::cout << "Tree idx1 height: " << computeHeight(tree[idx1]) << std::endl;


            // std::cout << "Tree idx0 internal nodes: " 
            //           << countInternalNodes(tree[idx0]) 
            //           << std::endl;

            // std::cout << "Tree idx0 leaf nodes: " 
            //           << countLeafNodes(tree[idx0]) 
            //           << std::endl;


            // std::cout << "Tree idx1 internal nodes: "
            //           << countInternalNodes(tree[idx1]) 
            //           << std::endl;

            // std::cout << "Tree idx1 leaf nodes: " 
            //           << countLeafNodes(tree[idx1]) 
            //           << std::endl;


#endif


            //std::cout << "Printing quads: " << std::endl;

            // if(tickNum == 49) {
            //     printQuads(idx0, true, agents, "idx0_xdim_quads.eps");
            //     printQuads(idx0, false, agents, "idx0_ydim_quads.eps");
            //     printQuads(idx1, true, agents, "idx1_xdim_quads.eps");
            //     printQuads(idx1, false, agents, "idx1_ydim_quads.eps");
            // }

            // // std::cout << "Tree 0" << std::endl;

            
            // if(tickNum == 1) {
            //     printSubTree(tree[idx0]);
            // }
            
            // std::cout << "Tree 1" << std::endl;

            // printSubTree(tree[idx1]);


#ifdef DEBUG_STRIPES
            LOG(DEBUG) << "Tuan: Tree0 at the end of start tick:" << std::endl;

            printSubTree(tree[idx0]);

            LOG(DEBUG) << "Tuan: Tree1 at the end of start tick:" << std::endl;

            printSubTree(tree[idx1]);

            LOG(DEBUG) << "Tuan: pointMap of Tree0 at the end of start tick: " << std::endl;

            tree[idx0]->printPointMap(idx0);

            LOG(DEBUG) << "Tuan: pointMap of Tree1 at the end of start tick: " << std::endl;

            tree[idx1]->printPointMap(idx1);
#endif

            //std::cout << std::endl;
            
        }

        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {
#ifdef DEBUG_STRIPES
            OptimizedStripesNode<U, K>::tuanTBD.clear();

            // if ((floor(r->getLow0()) == 1) &&
            //     (floor(r->getHigh0()) == 39) &&
            //     (floor(r->getLow1()) == 0) &&
            //     (floor(r->getHigh1()) == 36)) {

            // }
#endif
            
            // std::cout << "Tuan: AAAAAAAAAAAAAAAAAAAAA" << std::endl;
            // tree[0]->printPointMap(0);

            // printSubTree(tree[0]);
            // std::cout << "Tuan: BBBBBBBBBBBBBBBBBBBBB" << std::endl;

            // tree[1]->printPointMap(0);
            // printSubTree(tree[1]);



            //if (tickNum < OptimizedStripesINode<U, K>::LIFE_TIME)

            //std::cout << "tree[idx0]->size: " << tree[idx0]->size << std::endl;
            tree[idx0]->enumerateOverRegion(cb, r, tickNum, tickRef[idx0]);

#ifdef DEBUG_STRIPES
            LOG(DEBUG) << "Tuan: Tree0: The number of points has been reported for region : "
                       << *r << " is: " << OptimizedStripesNode<U, K>::tuanTBD.size() << std::endl;

            std::sort(OptimizedStripesNode<U, K>::tuanTBD.begin(),
                      OptimizedStripesNode<U, K>::tuanTBD.end(),
                      idlt<U, K>);

            for (int i = 0; i < OptimizedStripesNode<U, K>::tuanTBD.size(); i++) {
                LOG(DEBUG) << "\t" << *(OptimizedStripesNode<U, K>::tuanTBD[i]) << std::endl;
            }
            LOG(DEBUG) << "============================================" << std::endl;

            OptimizedStripesNode<U, K>::tuanTBD.clear();
#endif


            //std::cout << "tree[idx1]->size: " << tree[idx1]->size << std::endl;
            
            tree[idx1]->enumerateOverRegion(cb, r, tickNum, tickRef[idx1]);

#ifdef DEBUG_STRIPES
            LOG(DEBUG) << "Tuan: Tree1: The number of points has been reported for region : "
                       << *r << " is: " << OptimizedStripesNode<U, K>::tuanTBD.size() << std::endl;

            std::sort(OptimizedStripesNode<U, K>::tuanTBD.begin(),
                      OptimizedStripesNode<U, K>::tuanTBD.end(),
                      idlt<U, K>);

            for (int i = 0; i < OptimizedStripesNode<U, K>::tuanTBD.size(); i++) {
                LOG(DEBUG) << "\t" << *(OptimizedStripesNode<U, K>::tuanTBD[i]) << std::endl;
            }
            LOG(DEBUG) << "============================================" << std::endl;
#endif


        }

        void aggregateOverRegion(A* aggs, RType* r) {
            // TBD
        }

        void nearestK(IndexCallBack<U>* cb, U* u, int k) {
            // TBD
        }

        void afterUpdateAgg(U* u) {}

        void afterInsert(U* u) {
            LOG(DEBUG) << "Tuan: afterInsert:" << *u << std::endl;


            if(tickNum % (2 * lifeTime) < lifeTime) {
                //if (!transitionTick) {
                tree[idx0]->insertPoint(u, idx0, tickNum, tickRef[idx0]);
            } else {
                tree[idx1]->insertPoint(u, idx1, tickNum + 1, tickRef[idx1]);
            }

#ifdef DEBUG_STRIPES
            OptimizedStripesNode<U, K>::updatedPoints.push_back(u);
#endif
        }
        void beforeDelete(U* u) {
            LOG(DEBUG) << "Tuan: beforeDelete:" << *u << std::endl;

            if (!(tree[idx0]->deletePoint(K::getOID(u), idx0))) { // The point was not found in tree0
                LOG(DEBUG) << "Tuan: the point was not found in tree0." << std::endl;
                tree[idx1]->deletePoint(K::getOID(u), idx1);
            }

#ifdef DEBUG_STRIPES
            OptimizedStripesNode<U, K>::updatedPoints.push_back(u);
#endif
        }

        /**
         * Updates the velocity of the specified point. This method will be
         * called before the velocity has been updated in the master copy. The
         * new position and velocity are speciifed .
         */
        void beforeUpdateVelocity(U* point,
                                  typename K::KeyType0 new0,
                                  typename K::KeyType1 new1,
                                  typename K::KeyType0 newV0,
                                  typename K::KeyType1 newV1) {
#ifdef DEBUG_STRIPES
            OptimizedStripesNode<U, K>::updatedPoints.push_back(point);
#endif

            // if(tickNum == 0) {
            //     std::cout << "Updating point: " << *point
            //               << ", new0 = " << new0 << ", new1 = " << new1
            //               << ", newV0 = " << newV0 << ", newV1 = " << newV1 << std::endl;
            // }

            // Tuan:debugging
            //LOG(DEBUG) << "Tuan: Tree0 at the end of start tick:" << std::endl;
            //printSubTree(tree[0]);
            //LOG(DEBUG) << "Tuan: Tree1 at the end of start tick:" << std::endl;
            //printSubTree(tree[1]);

            if (!(tree[idx0]->deletePoint(K::getOID(point), idx0))) {
                //std::cout << "Tuan: the point was not found in tree0." << std::endl;
                tree[idx1]->deletePoint(K::getOID(point), idx1);
            }

            int index;

            if(tickNum % (2 * lifeTime) < lifeTime) {
                index = idx0;
            }
            else {
                index = idx1;
            }



            double transVx = newV0 + OptimizedStripesINode<U, K>::vxMax;
            double transVy = newV1 + OptimizedStripesINode<U, K>::vyMax;

            double pxRef = new0
                - (transVx - OptimizedStripesINode<U, K>::vxMax) * (tickNum - tickRef[index] + 1)
                + OptimizedStripesINode<U, K>::vxMax * OptimizedStripesINode<U, K>::LIFE_TIME;


            double pyRef = new1 - ((transVy - OptimizedStripesINode<U, K>::vyMax) 
                                   * (tickNum - tickRef[index] + 1));
            pyRef += OptimizedStripesINode<U, K>::vyMax * OptimizedStripesINode<U, K>::LIFE_TIME;


            // std::cout << "transVx = " << transVx 
            //           << " transVy = " << transVy
            //           << " pxRef = " << pxRef
            //           << " pyRef = " << pyRef
            //           << std::endl;

            tree[index]->insertPoint(point, transVx, transVy, pxRef, pyRef, index, tickNum, tickRef[index]);

            // if (!transitionTick) {
            //     // obtain transformed (V, Pref) tuple
            //     double transVx = newV0 + OptimizedStripesINode<U, K>::vxMax;
            //     double transVy = newV1 + OptimizedStripesINode<U, K>::vyMax;

            //     double pxRef = new0
            //         - (transVx - OptimizedStripesINode<U, K>::vxMax) * (tickNum - tickRef[idx0] + 1)
            //         + OptimizedStripesINode<U, K>::vxMax * OptimizedStripesINode<U, K>::LIFE_TIME;


            //     double pyRef = new1 - (transVy - OptimizedStripesINode<U, K>::vyMax) * (tickNum - tickRef[idx0] + 1)
            //         + OptimizedStripesINode<U, K>::vyMax * OptimizedStripesINode<U, K>::LIFE_TIME;

            //     // std::cout << "transVx = " << transVx 
            //     //           << " transVy = " << transVy
            //     //           << " pxRef = " << pxRef
            //     //           << " pyRef = " << pyRef
            //     //           << std::endl;

            //     tree[idx0]->insertPoint(point, transVx, transVy, pxRef, pyRef, idx0, tickNum, tickRef[idx0]);
            // } else {
                    
            //     // virtually consider the updates are for the next tick
            //     // obtain transformed (V, Pref) tuple
            //     double transVx = newV0 + OptimizedStripesINode<U, K>::vxMax;
            //     double transVy = newV1 + OptimizedStripesINode<U, K>::vyMax;
            //     double pxRef = new0 - (transVx - OptimizedStripesINode<U, K>::vxMax) * (tickNum + 1 - tickRef[idx1])
            //         + OptimizedStripesINode<U, K>::vxMax * OptimizedStripesINode<U, K>::LIFE_TIME;

            //     double pyRef = new1 - (transVy - OptimizedStripesINode<U, K>::vyMax) * (tickNum + 1 - tickRef[idx1])
            //         + OptimizedStripesINode<U, K>::vyMax * OptimizedStripesINode<U, K>::LIFE_TIME;

            //     tree[idx1]->insertPoint(point, transVx, transVy, pxRef, pyRef, idx1, tickNum + 1, tickRef[idx1]);
            // }
            //          }
                
            //std::cout << tree[idx0]->size << " " << tree[idx1]->size << std::endl;
                
        }

        void deleteSubtree(OptimizedStripesINode<U, K>* node) {
            DBUTL_ASSERT(node != NULL);

            for (int i = 0; i < OptimizedStripesINode<U, K>::NUM_CHILDREN; i++) {
                if (node->childrenPtr[i] != NULL) {
                    if (node->isLeaf[i]) {
                        delete node->childrenPtr[i];
                    } else {
                        deleteSubtree(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i]));
                    }
                }
            }
        }


        void endTick(bool force = false) {

#ifdef DEBUG_COUNT
            //printSubTree(tree[idx0]);

            //printSubTree(tree[idx0]);

            // std::cout << "idx0 = " << idx0 << std::endl;
            // std::cout << "idx1 = " << idx1 << std::endl;

            // std::cout << "transitionTick: " << transitionTick << std::endl;
            

            int count = 0;
            //printSubTree(tree[idx0], count);
            tree0_size.push_back(tree[idx0]->size/*count*/);
            // std::cout << "Tree0\t" << tickNum << "\t" << count << "size: " << tree[idx0]->size << std::endl;
            count = 0;
            printSubTree(tree[idx1], count);
            tree1_size.push_back(tree[idx1]->size/*count*/);

            numInternalNodeVisits.push_back(OptimizedStripesNode<U, K>::numInternalVisits);
            numLeafNodeVisits.push_back(OptimizedStripesNode<U, K>::numLeafVisits);

            numReportAll.push_back(OptimizedStripesNode<U, K>::numReport);

            //std::cout << "numVisits: " << OptimizedStripesNode<U, K>::numVisits << std::endl;
            OptimizedStripesNode<U, K>::numInternalVisits = 0;
            OptimizedStripesNode<U, K>::numLeafVisits = 0;

            OptimizedStripesNode<U, K>::numReport = 0;
            // std::cout << "Tree1\t" << tickNum << "\t" << count << "size: " << tree[idx1]->size << std::endl;

            // std::cout << "overlapCount: " << OptimizedStripesINode<U, K>::overlapCount << std::endl;
            // std::cout << "disjunctCount: " << OptimizedStripesINode<U, K>::disjunctCount << std::endl;
#endif



            if (force) {

                idx0 = 0;
                idx1 = 1;

                OptimizedStripesINode<U, K>::pointMap[idx0].clear();
                OptimizedStripesINode<U, K>::pointMap[idx1].clear();

                if (tree[idx0] != NULL) {
                    deleteSubtree(tree[idx0]);
                }

                if (tree[idx1] != NULL) {
                    deleteSubtree(tree[idx1]);
                }

                OptimizedStripesLNode<U, K>::nextlid = 0;
                OptimizedStripesINode<U, K>::nextnid = 0;

                tickNum = 0;
                tickRef[idx0] = 0;
                tickRef[idx1] = 0;

                transitionTick = false;


                return;
            }
#ifdef DEBUG_STRIPES
            LOG(DEBUG) << "Tuan: updated points in tick " << tickNum << " are: " << std::endl;
            for (int i = 0; i < OptimizedStripesNode<U, K>::updatedPoints.size(); i++) {
                LOG(DEBUG) << "\t\t" << *(OptimizedStripesNode<U, K>::updatedPoints[i]) << std::endl;
            }

            OptimizedStripesNode<U, K>::updatedPoints.clear();
#endif




            LOG(DEBUG) << "Tuan: starting endTick procedure (tickNum = " << tickNum << ")..." << std::endl;




            tickNum++;
            std::cout << "tickNum: " << tickNum << std::endl;
            int modl = tickNum % lifeTime;
            int mod2l = tickNum % (2*lifeTime);



            if(modl == 0) {
                int delIndex;

                // At tick 2k * lifetime. Clear out idx0 and move to idx1.
                if(mod2l == 0) {
                    delIndex = idx0;
                }
                // At tick (2k+1) * lifetime. Clear out idx1 and move to idx0. 
                else {
                    DBUTL_ASSERT((mod2l == lifeTime));
                    delIndex = idx1;
                }

                std::vector<U*> unupdatedPoints;
                int numDeleted
                    = tree[delIndex]->deleteUnupdatedPoints(&unupdatedPoints, delIndex);
                tree[delIndex]->size -= numDeleted;
                
                DBUTL_ASSERT((tree[delIndex]->size == 0));

                for (int i = 0; i < unupdatedPoints.size(); i++) {
                    U* point = unupdatedPoints[i];
                    tree[1 - delIndex]->insertPoint(point, 
                                                    1 - delIndex, 
                                                    tickNum, 
                                                    tickRef[1 - delIndex]);
                }

                tickRef[delIndex] = tickNum;
                // std::cout << "Updating index " << delIndex 
                //           << " refTime to " << tickNum << std::endl;
            }






            // set the flag to trigger all the updates that
            // this tick is the transition from tree0 to tree1,
            // i.e., delete points from tree0, insert to tree1
	    //std::cout << "OptimizedStripesINode<U, K>::LIFE_TIME): " << OptimizedStripesINode<U, K>::LIFE_TIME << std::endl;
//             if (((tickNum + 1) % OptimizedStripesINode<U, K>::LIFE_TIME) == 0) {
//                 transitionTick = true;
//        //         std::cout << "Transitioning..." << std::endl;





//             } else if ((tickNum % OptimizedStripesINode<U, K>::LIFE_TIME) == 0) {
//                 transitionTick = false;

//  #ifdef DEBUG_STRIPES
//                 LOG(DEBUG) << "Tuan: reinserting unupdated points:" << std::endl;
//                 LOG(DEBUG) << "Tuan: the tree before re-insertion is: " << std::endl;
//                 printSubTree(tree[idx0]);
// #endif

//                 std::vector<U*> unupdatedPoints;
//                 int numDeleted = tree[idx0]->deleteUnupdatedPoints(&unupdatedPoints);
//                 tree[idx0]->size -= numDeleted;
                

// //                std::cout << "\t # un-updated points : " << unupdatedPoints.size() << std::endl;


// #ifdef DEBUG_STRIPES
//                 LOG(DEBUG) << "Tuan: un-updated points are: " << std::endl;

//                 for (int i = 0; i < unupdatedPoints.size(); i++) {
//                     LOG(DEBUG) << "\t\t" << *(unupdatedPoints[i]) << std::endl;
//                 }
// #endif


//                 LOG(DEBUG) << "Tuan: re-insert unupdated points:" << std::endl;


// #ifdef DEBUG_STRIPES
//                 LOG(DEBUG) << "Tuan: the tree after re-insertion is: " << std::endl;
//                 printSubTree(tree[idx0]);
// #endif

//                 // swap the 2 roots
//                 idx0 = 1 - idx0;
//                 idx1 = 1 - idx1;

//                 LOG(DEBUG) << "Tuan: update tickRef of Tree0 from " << tickRef[idx0] << " to " << tickNum << std::endl;
//                 tickRef[idx0] = tickNum;

//                 LOG(DEBUG) << "Tuan: update tickRef of Tree1 from " << tickRef[idx1] << " to "
//                            << tickNum + OptimizedStripesINode<U, K>::LIFE_TIME << std::endl;
//                 tickRef[idx1] = tickNum + OptimizedStripesINode<U, K>::LIFE_TIME;

//                 // swap the point maps


//             }

//             LOG(DEBUG) << "Tuan: procedure endTick has finished." << std::endl;
         }

    private:
         static const int NUM_DIM;

         int tickNum;
         int tickRef[2];
         OptimizedStripesINode<U, K>** tree;


         /**
          * Prints a tree using the format:
          *    node
          *    .....
          *    indexi: internal node
          *    .....
          *    indexj: leafnode
          *    .
          *    .
          *    .
          *        ------------------------
          *    indexi: internal node
          *
          *        ------------------------
          *    .
          *    .
          *    .
          */
         void printSubTree(OptimizedStripesINode<U, K>* node) {

             if (node == NULL) return;
             
             //LOG(DEBUG) << "\t" << *node << std::endl;
             std::cout << "\t" << *node << std::endl;

             std::vector<OptimizedStripesINode<U, K>* > nodes;
             for (int i = 0; i < OptimizedStripesINode<U, K>::NUM_CHILDREN; i++) {
//                 std::cout << "starting loop" << std::endl;
                 
                 if (node->childrenPtr[i] != NULL) {
                     if (node->isLeaf[i]) {
                         std::cout << "\t\t" << i << " :Leaf : " << (*(dynamic_cast<OptimizedStripesLNode<U, K>*>(node->childrenPtr[i])));
                         std::cout << std::endl;
                     } else {
                         std::cout << "\t\t" << i << " :INode: " << *(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i])) << std::endl;
                         nodes.push_back(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i]));
                     }
                 }

//                 std::cout << "ending loop" << std::endl;

             }
             std::cout << "\t\t---------------------------------------" << std::endl;
             typename std::vector<OptimizedStripesINode<U, K>* >::const_iterator iter;
             for(iter = nodes.begin(); iter != nodes.end(); iter++) {
                 printSubTree(*iter);
             }
         }

        /**
         * Returns the number of points in the subtree rooted at node. 
         */
        void printSubTree(OptimizedStripesINode<U, K>* node, int& count) {
            if (node == NULL) return;

            LOG(DEBUG) << "\t" << *node << std::endl;

             std::vector<OptimizedStripesINode<U, K>* > nodes;

             //std::cout << "NUM_CHILDREN: " << OptimizedStripesINode<U, K>::NUM_CHILDREN << std::endl;
             

             for (int i = 0; i < OptimizedStripesINode<U, K>::NUM_CHILDREN; i++) {
                 if (node->childrenPtr[i] != NULL) {
                     if (node->isLeaf[i]) {
                         LOG(DEBUG) << "\t\t" << i << " :Leaf : " << *(dynamic_cast<OptimizedStripesLNode<U, K>*>(node->childrenPtr[i])) << std::endl;
                         count += (dynamic_cast<OptimizedStripesLNode<U, K>*>(node->childrenPtr[i]))->size;

                         
                     } else {
                         LOG(DEBUG) << "\t\t" << i << " :INode: " << *(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i])) << std::endl;
                         nodes.push_back(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i]));
                     }
                 }
             }
             LOG(DEBUG) << "\t\t---------------------------------------" << std::endl;
             typename std::vector<OptimizedStripesINode<U, K>* >::const_iterator iter;
             for(iter = nodes.begin(); iter != nodes.end(); iter++) {
                 printSubTree(*iter, count);
             }
         }

        
        int countInternalNodes(OptimizedStripesINode<U, K>* node) {
            int count = 1;

            for (int i = 0; i < OptimizedStripesINode<U, K>::NUM_CHILDREN; i++) {
                if (node->childrenPtr[i] != NULL && !node->isLeaf[i]) {
                    count += countInternalNodes(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i]));
                }
            }
            return count;
        }

        int countLeafNodes(OptimizedStripesINode<U, K>* node) {
            int count = 0;

            for (int i = 0; i < OptimizedStripesINode<U, K>::NUM_CHILDREN; i++) {
                if (node->childrenPtr[i] != NULL && node->isLeaf[i]) {
                    count += 1;
                }
                else if(!node->isLeaf[i]){
                    count += countLeafNodes(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i]));
                }
            }
            return count;
        }


        int computeHeight(OptimizedStripesINode<U, K>* node) {
            int nextLevelMax = 0;
            for(int i = 0; i < OptimizedStripesINode<U, K>::NUM_CHILDREN; i++) {
                if (node->childrenPtr[i] != NULL) {

                    int currHeight = 0;

                    if(node->isLeaf[i]) {
                        currHeight = 1;
                    }
                    else {
                        currHeight = computeHeight(dynamic_cast<OptimizedStripesINode<U, K>*>(node->childrenPtr[i]));
                    }
                    if(currHeight > nextLevelMax) {
                        nextLevelMax = currHeight;
                    }
                }
            }
            return nextLevelMax + 1;
        }


   };
    template <typename U, typename K, typename A>
    const int OptimizedStripes<U, K, A>::NUM_DIM = 2;
}

#endif

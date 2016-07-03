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

#ifndef _STRIPES_H_
#define _STRIPES_H_

//#include <tr1/unordered_map>
#include <unordered_map>

#include "2D/MovingSpatialIndex2D.h"
#include "2D/IndexUtils2D.h"
#include <iomanip>
namespace PSimIndex {

    template<typename U, typename K, typename A>
    class Stripes : public MovingSpatialIndex2D<U, K, A> {


    //------------------------------------------------------------------------
    // Static Constants
    //------------------------------------------------------------------------

    private:

        /**
         * The maximum number of data points stored in a leaf node. 
         */
        static const int MAXENTRIES = 4;

        /**
         * The number of children of an internal node. 
         */
        static const int NUM_CHILDREN = 16;

    //------------------------------------------------------------------------
    // Class Declarations
    //------------------------------------------------------------------------

    private:
        class InternalNode;
        

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        typedef std::pair<InternalNode*, int> ChildIndex;
        //typedef std::tr1::unordered_map<typename K::IDType, ChildIndex*> PointMap;
        typedef std::unordered_map<typename K::IDType, ChildIndex*> PointMap;


        enum QueryRelation {INSIDE, DISJUNCT, OVERLAP}; 


        //TODO: Use template types instead of double?
        struct Grid {
            double vx, px;
            double vy, py;
            
            Grid() {
                vx = vy = 0;
                px = py = 0;
            }
            Grid( double vx, double px, double vy, double py) 
                : vx(vx), px(px), vy(vy), py(py) 
            {}
        };

        struct TransformedPoint {
            double transVx;
            double transVy;
            double pxRef;
            double pyRef;

            TransformedPoint() {
                transVx = transVy = 0;
                pxRef = pyRef = 0;
            }

            TransformedPoint(double vx, double vy, double px, double py) 
                : transVx(vx), transVy(vy), pxRef(px), pyRef(py) {}

            TransformedPoint* clone() {
                return new TransformedPoint(transVx, transVy, pxRef, pyRef);
            }

            friend std::ostream& 
            operator<<(std::ostream &out, const TransformedPoint& tp) {
                out << "TransformedPoint(" << tp.transVx << ", " << tp.transVy
                    << ", " << tp.pxRef << ", " << tp.pyRef << ")";
                return out;
            }

        };




        /**
         * Global parameters that need to be passed to internal nodes. 
         */

        // TODO: Make tickNum a parameter. 
        struct GlobalParams {
            int vxMax;
            int vyMax;
            int pxMax;
            int pyMax;
            int lifeTime;
            PointMap* pointMap;

            GlobalParams(int _vxMax, int _vyMax, 
                         int _pxMax, int _pyMax, 
                         int _lifeTime, PointMap* _pointMap)
                : vxMax(_vxMax), vyMax(_vyMax), 
                  pxMax(_pxMax), pyMax(_pyMax), 
                  lifeTime(_lifeTime), pointMap(_pointMap)
            {}
        };


        /**
         *
         *
         */
        class Node {

        protected:
            static int nextID;

            // non-zero: indicates the level of the non-leaf node, zero:
            // indicates this node is a leaf node
            uint16_t level;  

            // non-leaf node: indicates total number of actual data entries
            // stored in all the leaf nodes in all the leaf nodes in the
            // subtree below this non-leaf node.  leaf-node: indicates total
            // number of data points stored in this leaf node.

            Grid grid;

            int id;

        public:
            static void resetId() { nextID = 0; }
            
            uint32_t size;  

            Node() : level(0), size(0) { id = nextID++; }
            Node(int level) : level(level), size(0) { id = nextID++; }
            Node(double vx, double px, double vy, double py);
            virtual ~Node() {}

            void setGrid(double vx, double px, double vy, double py);


        };
        

        /**
         *
         *
         *
         */
        class LeafNode : public Node {
        public:
            //TODO: Make these private?
            U** entries;
            TransformedPoint** transEntries;


            LeafNode(int level = 0);
            ~LeafNode();


            bool isFull() const { return (Node::size == MAXENTRIES); } 
            bool isEmpty() const { return (Node::size == 0); } 

            void insertPoint(U* point, TransformedPoint* t);
            bool deletePoint(typename K::IDType oid);
            void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) const;




            //TODO: Is there any way to separate the definition of this function? 
            friend std::ostream& 
            operator<<(std::ostream &out, const LeafNode& node) {

                out << "LeafNode(" 
                    << std::setiosflags(std::ios::fixed) << std::setprecision(2)
                    << node.id << ", " << node.size << ", "<< node.level << ", " << node.size << ", "
                    << "{(" << node.grid.vx << "," << node.grid.px << "), (" 
                    << node.grid.vy << "," << node.grid.py << ")})" << std::endl;
            

                for (int i = 0; i <node.size; i++) {
                    out << "\t\t\t\t" << *(node.entries[i]) 
                        << "\t" << *(node.transEntries[i]);
                    if(i != node.size)
                        out << std::endl;
                }
                return out;
            }
        };


        /**
         *
         *
         *
         */
        class InternalNode : public Node {
        private:
            
            GlobalParams* gp;

            InternalNode* parentPtr;



            static int PD(double pRef, double pRefQuad, double SL_P);
            static int VD(double V, double VQuad, double SL_V);

            double getPref(double p, double V, double vMax, 
                           int tickNum, int tickRef) const;
            double getPref(double p, double v) const;

            bool isBelow(double ur_v, double ur_p, double pl, double vMax, 
                         int tickNum, int tickRef) const;

            bool isAbove(double ll_v, double ll_p, double pu, double vMax, 
                         int tickNum, int tickRef) const;


            bool isOnOrBelow(double ur_v, double ur_p, double pu, double vMax, 
                             int tickNum, int tickRef) const;

            bool isOnOrAbove(double ll_v, double ll_p, double pl, double vMax, 
                             int tickNum, int tickRef) const;


            int getRelativePosition(RType* r, int tickNum, int tickRef) const;

            void reportAll(IndexCallBack<U>* cb) const;

        public:
            Node** childrenPtr;
            bool* isLeaf;


            InternalNode(GlobalParams* gp, int level = 0, 
                         InternalNode* parent = NULL);
            ~InternalNode();

            void insertPoint(U* p, int treeRef, int tickNum, int tickRef);
            void insertPoint(U* p, TransformedPoint* tp, int treeRef, 
                             int tickNum , int tickRef);


            void collectEntries(std::vector<U*>* collectedEntries);

            bool deletePoint(typename K::IDType oid, int childIndex, int treeRef);
            bool deletePoint(typename K::IDType oid, int treeRef);


            void enumerateOverRegion(IndexCallBack<U>* cb, RType* r,
                                     int tickNum, int tickRef) const;


            int deleteUnupdatedPoints(std::vector<U*>* unupdatedPoints, 
                                      int treeRef);
            
            void drawQuads(FILE* gp, bool xDim);


            //TODO: Is there any way to separate the definition of this function? 
            friend std::ostream& 
            operator<<(std::ostream &out, const InternalNode& node) {
                out << "InternalNode(" << std::setiosflags(std::ios::fixed)
                    << std::setprecision(2) << node.id << ", " << node.size 
                    << ", " << node.level << ", " << "{(" << node.grid.vx
                    << "," << node.grid.px << "), ("  << node.grid.vy << "," 
                    << node.grid.py << ")}" << ", " << node.size << ")";
                return out;
            }
        };


    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:

        std::vector<U*>* agentsCopy;

        char name[100];

        InternalNode* tree[2];

        uint32_t tickRef[2];
        uint32_t tickNum;

        int plotCount;
        std::vector<RType*> queriesToPlot;

        PointMap pointMap[2];
        GlobalParams* gp;



        void deleteSubtree(InternalNode* node);

        int countSubTree(InternalNode* node) const;
        void printSubTree(InternalNode* node) const;
        int countInternalNodes(InternalNode* node) const;
        int countLeafNodes(InternalNode* node) const;
        int computeHeight(InternalNode* node) const;


    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------
    public:

        //tmp.
        static bool toPrint;
        static int plotTick;

        Stripes(int vxMax = 600, int vyMax = 600,
                int pxMax = 577351, int pyMax = 577351,
                int lifeTime = 2);

        ~Stripes();

        static const char* getClassName() { return "Stripes"; }
        const char* getInstanceName() { return name; }


        void startTick(std::vector<U*>* agents);
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);




        void afterInsert(U* u);
        void beforeDelete(U* u);
        void beforeUpdateVelocity(U* point,
                                  typename K::KeyType0 new0,
                                  typename K::KeyType1 new1,
                                  typename K::KeyType0 newV0,
                                  typename K::KeyType1 newV1);


        void endTick(bool force = false);



        void drawQueries(FILE* gp, int index, bool xDim, std::vector<RType*>* queries);

        void drawTree(int index, bool xDim, 
                      std::vector<U*>* agents, 
                      const char* name, 
                      std::vector<RType*>* queries = NULL);



        // Unimplemented Methods.
        void aggregateOverRegion(A* aggs, RType* r) {}
        void nearestK(IndexCallBack<U>* cb, U* u, int k) {}
        void afterUpdateAgg(U* u) {}





    };

        



    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Implementation Starts Here
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------


    //------------------------------------------------------------------------
    // Node Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    Stripes<U,K,A>::Node::Node(double vx, double px, double vy, double py) 
        : level(0), size(0) {
        id = nextID++;
        setGrid(vx, px, vy, py);
    }

    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::Node::setGrid(double vx, double px, 
                                       double vy, double py) {
        grid.vx = vx;
        grid.px = px;
        grid.vy = vy;
        grid.py = py;
    }


    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::Node::nextID = 0;

    //------------------------------------------------------------------------
    // LeafNode Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    Stripes<U,K,A>::LeafNode::LeafNode(int level) : Node(level) {
        entries = new U*[MAXENTRIES];
        transEntries = new TransformedPoint*[MAXENTRIES];
    }

    template<typename U, typename K, typename A>
    Stripes<U,K,A>::LeafNode::~LeafNode() {
        delete [] entries;

        for(int i = 0; i < Node::size; i++) {
            delete transEntries[i];
        }

        delete [] transEntries;
    }

    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::LeafNode::insertPoint(U* point, TransformedPoint* t) {
        DBUTL_ASSERT(Node::size < MAXENTRIES);
        entries[Node::size] = point;
        transEntries[Node::size] = t;
        Node::size++;
    }

    template<typename U, typename K, typename A>
    bool Stripes<U,K,A>::LeafNode::deletePoint(typename K::IDType oid) {

        //std::cout << "In LeafNode::deletePoint " << oid << std::endl;

        bool found = false;
        int i = 0;
        for (; i < Node::size; i++) {
            // Tuan: since we regenerated the old point, we have to test equality using oid
            if (K::getOID(entries[i]) == oid) {
                found = true;
                break;
            }
        }

        // TODO: delete transEntries[i]
        i++;
        for (; i < Node::size; i++) {
            entries[i-1] = entries[i];
            transEntries[i-1] = transEntries[i];
        }
        
        if (found) {
            Node::size--;
        }

        return found;
    }

    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::LeafNode::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                       RType* r) const {


        LOG(DEBUG) << "In leaf enumerateOverRegion size=" << Node::size << std::endl;

        // if(Stripes<U,K,A>::toPrint)
        //     std::cout << "Enumerating over leaf " << *this << std::endl;


        for (int i = 0; i < Node::size; i++) {
            U* point  = entries[i];

            LOG(DEBUG) << "Comparing point" << *point << " with region " << *r << std::endl;

            if (r->template containsPoint<U,K>(point)) {
                cb->reportPoint(entries[i]);
            }
        }
    }






    //------------------------------------------------------------------------
    // InternalNode Methods
    //------------------------------------------------------------------------


    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::InternalNode::PD(double pRef, 
                                         double pRefQuad, 
                                         double SL_P) {
        return (((int)ceil((pRef - pRefQuad) / (SL_P / 2))) -1);
    }

    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::InternalNode::VD(double V, double VQuad, double SL_V) {
        return (((int)ceil((V - VQuad) / (SL_V / 2))) - 1);
    }


    template<typename U, typename K, typename A>
    double Stripes<U,K,A>::InternalNode::getPref(double p, double V, double vMax, 
                                             int tickNum, int tickRef) const {
        return  p - (V - vMax) * (tickNum - tickRef) + vMax * gp->lifeTime;
    }

    template<typename U, typename K, typename A>
    double Stripes<U,K,A>::InternalNode::getPref(double p, double v) const {
        return p + v * gp->lifeTime;
    }
    

    template<typename U, typename K, typename A>
    bool Stripes<U,K,A>::InternalNode::isBelow(double ur_v, double ur_p, 
                                               double pl, double vMax, 
                                               int tickNum, int tickRef) const {


        double Pref = getPref(pl, ur_v, vMax, tickNum, tickRef);

//         if(Stripes<U,K,A>::toPrint) {
//             std::cout << "In isBelow: " << Pref << std::endl;
//         }

        if (Pref > ur_p) {
            return true;
        } else {
            return false;
        }
    }

    template<typename U, typename K, typename A>
    bool Stripes<U,K,A>::InternalNode::isAbove(double ll_v, double ll_p, 
                                               double pu, double vMax, 
                                               int tickNum, int tickRef) const {
        double Pref = getPref(pu, ll_v, vMax, tickNum, tickRef);


//         if(Stripes<U,K,A>::toPrint) {
//             std::cout << "In isAbove: " << Pref << std::endl;
//         }

        
        if (Pref < ll_p) {
            return true;
        } else {
            return false;
        }
    }

    template<typename U, typename K, typename A>
    bool Stripes<U,K,A>::InternalNode::isOnOrBelow(double ur_v, double ur_p, 
                                                   double pu, double vMax, 
                                                   int tickNum, int tickRef) const {
        double Pref = getPref(pu, ur_v, vMax, tickNum, tickRef);
        if (Pref >= ur_p) {
            return true;
        } else {
            return false;
        }
    }


    template<typename U, typename K, typename A>
    bool Stripes<U,K,A>::InternalNode::isOnOrAbove(double ll_v, double ll_p, 
                                                   double pl, double vMax, 
                                                   int tickNum, int tickRef) const {
        double Pref = getPref(pl, ll_v, vMax, tickNum, tickRef);
            
        if (Pref <= ll_p) {
            return true;
        } else {
            return false;
        }
    }

    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::InternalNode::getRelativePosition(RType* r, 
                                                          int tickNum, 
                                                          int tickRef) const {

//         if(Stripes<U,K,A>::toPrint)
//             std::cout << "region " << *r << " tickNum "  << tickNum << " tickRef " << tickRef << std::endl;

        // Compute relative position on x plane. 

        double pl_x = r->getLow0();
        double pu_x = r->getHigh0();

        double ll_vx = Node::grid.vx;
        double ll_px = Node::grid.px;

        double SL_Px = ((double)(gp->pxMax + 2 * gp->vxMax * gp->lifeTime) 
                        / (1 << Node::level));

        double SL_Vx = (double)(2 * gp->vxMax) / (1 << Node::level);

        double ur_vx = ll_vx + SL_Vx;
        double ur_px = ll_px + SL_Px;

        int relPosX;

        if (isOnOrAbove(ll_vx, ll_px, pl_x, gp->vxMax, tickNum, tickRef) &&
            isOnOrBelow(ur_vx, ur_px, pu_x, gp->vxMax, tickNum, tickRef)) {
            relPosX = INSIDE;

//             if(Stripes<U,K,A>::toPrint)
//                 std::cout << "relPosX = INSIDE " << std::endl;


        } else if (isAbove(ll_vx, ll_px, pu_x, gp->vxMax, tickNum, tickRef) ||
                   isBelow(ur_vx, ur_px, pl_x, gp->vxMax, tickNum, tickRef)) {
            relPosX = DISJUNCT;

//             if(Stripes<U,K,A>::toPrint)
//                 std::cout << "relPosX = DISJUNCT " << std::endl;

        } else {
            relPosX = OVERLAP;

//             if(Stripes<U,K,A>::toPrint)
//                 std::cout << "relPosX = OVERLAP " << std::endl;

        }


        // Compute relative position on y plane. 

        double pl_y = r->getLow1();
        double pu_y = r->getHigh1();
        double ll_vy = Node::grid.vy;
        double ll_py = Node::grid.py;

        double SL_Py = ((double)(gp->pyMax + 2 * gp->vyMax * gp->lifeTime) 
                        / (1 << Node::level));




        
        double SL_Vy = (double)(2 * gp->vyMax) / (1 << Node::level);

        double ur_vy = ll_vy + SL_Vy;
        double ur_py = ll_py + SL_Py;


//         if(Stripes<U,K,A>::toPrint) {
//             std::cout << "level: " << Node::level << std::endl;

//             double foo = (double)(gp->pyMax + 2 * gp->vyMax * gp->lifeTime);

//             std::cout << "(double)(gp->pyMax + 2 * gp->vyMax * gp->lifeTime: " << foo << std::endl;


//             std::cout << "tickRef: " << tickRef << " tickNum: " << tickNum << std::endl;

//             std::cout << "testing rel pos on the Y plane: " << std::endl
//                       << "\t\tpl_y = " << pl_y << std::endl
//                       << "\t\tpu_y = " << pu_y << std::endl
//                       << "\t\tSL_Vy = " << SL_Vy << std::endl
//                       << "\t\tSL_Py = " << SL_Py << std::endl
//                       << "\t\tll_vy = " << ll_vy << std::endl
//                       << "\t\tll_py = " << ll_py << std::endl
//                       << "\t\tur_vy = " << ur_vy << std::endl
//                       << "\t\tur_py = " << ur_py << std::endl;

        
//         }


        int relPosY;
        if (isOnOrAbove(ll_vy, ll_py, pl_y, gp->vyMax, tickNum, tickRef) &&
            isOnOrBelow(ur_vy, ur_py, pu_y, gp->vyMax, tickNum, tickRef)) {
            relPosY = INSIDE;

//             if(Stripes<U,K,A>::toPrint)
//                 std::cout << "relPosY = INSIDE " << std::endl;

        } else if (isAbove(ll_vy, ll_py, pu_y, gp->vyMax, tickNum, tickRef) ||
                   isBelow(ur_vy, ur_py, pl_y, gp->vyMax, tickNum, tickRef)) {
            relPosY = DISJUNCT;

//             if(Stripes<U,K,A>::toPrint)
//                 std::cout << "relPosY = DISJUNCT " << std::endl;

        } else {
            relPosY = OVERLAP;

//             if(Stripes<U,K,A>::toPrint)
//                 std::cout << "relPosY = OVERLAP " << std::endl;

        }

        // Combine relative position on both planes. 

        if ((relPosX == INSIDE) && (relPosY == INSIDE)) {
            return INSIDE;
        } else if ((relPosX == DISJUNCT) || (relPosY == DISJUNCT)) {
            return DISJUNCT;
        } else {
            return OVERLAP;
        }
    }

    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::InternalNode::reportAll(IndexCallBack<U>* cb) const {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (childrenPtr[i] != NULL) {
                if (isLeaf[i]) {
                    LeafNode* leaf = dynamic_cast<LeafNode*>(childrenPtr[i]);
                        for (int j = 0; j < leaf->size; j++) {
                            cb->reportPoint(leaf->entries[j]);
                        }
                }
                else {
                    InternalNode* inode = dynamic_cast<InternalNode*>(childrenPtr[i]);
                    inode->reportAll(cb);
                }
            }
        }
    }


    template<typename U, typename K, typename A>
    Stripes<U,K,A>::InternalNode::InternalNode(GlobalParams* _gp, 
                                               int level, 
                                               InternalNode* parent) 
        : Node(level), gp(_gp), parentPtr(parent) {
        
        childrenPtr = new Node*[NUM_CHILDREN];
        isLeaf = new bool[NUM_CHILDREN];

        for(int i = 0; i < NUM_CHILDREN; i++) {
            childrenPtr[i] = NULL;
            isLeaf[i] = true;
        }

    }
    template<typename U, typename K, typename A>
    Stripes<U,K,A>::InternalNode::~InternalNode() {
        delete [] childrenPtr;
        delete [] isLeaf;
    }




    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::InternalNode::insertPoint(U* p, int treeRef, 
                                                   int tickNum, int tickRef) {
        double new0 = K::getKey0(p);
        double new1 = K::getKey1(p);
        double newV0 = K::getV0(p);
        double newV1 = K::getV1(p);

        // obtain transformed (V, Pref) tuple
        double transVx = newV0 + gp->vxMax;
        double transVy = newV1 + gp->vyMax;
        double pxRef = new0 - (transVx - gp->vxMax) * (tickNum - tickRef)
            + gp->vxMax * gp->lifeTime;



        double pyRef = new1 - (transVy - gp->vyMax) * (tickNum - tickRef)
            + gp->vyMax * gp->lifeTime;

        TransformedPoint* tp = new TransformedPoint(transVx, transVy, 
                                                    pxRef, pyRef);



//         if(K::getOID(p) == 6071) {
//             std::cout << "tickNum: " << tickNum << " tickRef: " << tickRef << std::endl;
//             std::cout << "inserting: " << *p << " treeRef " << treeRef << std::endl;
//             std::cout << "transformed: " << *tp << std::endl;
//         }

        insertPoint(p, tp, treeRef, tickNum, tickRef);


    }


    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::InternalNode::insertPoint(U* p, 
                                                   TransformedPoint* tp, 
                                                   int treeRef, 
                                                   int tickNum, 
                                                   int tickRef) {

        // increment the size of the node
        Node::size++;

        double SL_Px = ((double)(gp->pxMax + 2 * gp->vxMax * gp->lifeTime)
                        / (1 << Node::level));
        double SL_Py = ((double)(gp->pyMax + 2 * gp->vyMax * gp->lifeTime) 
                        / (1 << Node::level));

        double SL_Vx = (double)(2 * gp->vxMax) / (1 << Node::level);
        double SL_Vy = (double)(2 * gp->vyMax) / (1 << Node::level);


        DBUTL_ASSERT((tp->transVx >= Node::grid.vx));
        DBUTL_ASSERT((tp->transVy >= Node::grid.vy));
        DBUTL_ASSERT((tp->pxRef >= Node::grid.px));
        DBUTL_ASSERT((tp->pyRef >= Node::grid.py));

        int PD3 = PD(tp->pxRef, Node::grid.px, SL_Px);
        int VD2 = VD(tp->transVx, Node::grid.vx, SL_Vx);
        int PD1 = PD(tp->pyRef, Node::grid.py, SL_Py);
        int VD0 = VD(tp->transVy, Node::grid.vy, SL_Vy);

        int index = 8 * PD3 + 4 * VD2 + 2 * PD1 + VD0;

        // the child pointer points to a leaf node
        if (isLeaf[index]) { 

            // case 1: the target leaf node is non-existent.
            if (childrenPtr[index] == NULL) { 

                Node* leafNode = new LeafNode(Node::level + 1);

                // TODO: should the new entry be the vectors (V,Pref)
                dynamic_cast<LeafNode*>(leafNode)->insertPoint(p, tp);
                childrenPtr[index] = leafNode;

                ChildIndex* childNodeIndex = new ChildIndex(this, index);
                gp->pointMap[treeRef][K::getOID(p)] = childNodeIndex;
            }

            // Target leaf exists. 
            else {
                LeafNode* leafNode
                    = dynamic_cast<LeafNode*>(childrenPtr[index]);

                // case 3: the target leaf node is found and is full.
                if (leafNode->isFull()) { 

                    InternalNode* iNode
                        = new InternalNode(gp, Node::level + 1, this);

                    iNode->setGrid(Node::grid.vx + (double)VD2 * SL_Vx / 2,
                                   Node::grid.px + (double)PD3 * SL_Px / 2,
                                   Node::grid.vy + (double)VD0 * SL_Vy / 2,
                                   Node::grid.py + (double)PD1 * SL_Py / 2);

                    

                    // Distribute old entries to new node. 
                    for (int i = 0; i < leafNode->size; i++) {
                        U* point = leafNode->entries[i];
                        
                        iNode->insertPoint(leafNode->entries[i],
                                           leafNode->transEntries[i]->clone(),
                                           treeRef, tickNum, tickRef);
                    }


                    iNode->insertPoint(p, tp, treeRef, tickNum, tickRef);
                    
                    childrenPtr[index] = iNode;
                    isLeaf[index] = false;

                    delete leafNode;
                }
                // case 2: the target leaf node is found and is not full.
                else {
                    leafNode->insertPoint(p, tp);
                    
                    ChildIndex* childNodeIndex = new ChildIndex(this, index);
                    gp->pointMap[treeRef][K::getOID(p)] = childNodeIndex;
                }
            }
        }
        else {
            InternalNode* iNode
                = dynamic_cast<InternalNode*>(childrenPtr[index]);

            iNode->insertPoint(p, tp, treeRef, tickNum, tickRef);

        }
    }


    template<typename U, typename K, typename A>
    bool Stripes<U,K,A>::InternalNode::deletePoint(typename K::IDType oid, 
                                                   int childIndex, 
                                                   int treeRef) {

        //std::cout << "In InternalNode::deletePoint 1 " << oid << std::endl;

        //TODO: What is this case??
/*        if(Node::size == 1 && childIndex == 1) {
            std::cout << "case 1" << std::endl;
            return false;
            }*/

        if (childrenPtr[childIndex] == NULL) {
            LOG(ERR) << "Stripes::InternalNode::deletePoint: "
                     << "Point was not found at index " << childIndex
                     << " as expected" << std::endl;
            return false;
        }

        DBUTL_ASSERT(isLeaf[childIndex]);
        LeafNode* leaf = dynamic_cast<LeafNode*>(childrenPtr[childIndex]);                


        bool ans = leaf->deletePoint(oid);
    
        if (ans == false) return false;
        gp->pointMap[treeRef][oid] = NULL;

        // If the leaf is empty, then we delete it from the tree. 
        if (leaf->isEmpty()) {
            delete leaf;
            childrenPtr[childIndex] = NULL;
        }

        //Adjust size on path to root. 
        Node::size--;
        InternalNode* parent = parentPtr;
        while(parent != NULL) {
            parent->size--;
            parent = parent->parentPtr;
        }

        // TODO: Deal with underfull node.

    }
    

    template<typename U, typename K, typename A>
    bool Stripes<U,K,A>::InternalNode::deletePoint(typename K::IDType oid, 
                                                   int treeRef) {
        //std::cout << "In InternalNode::deletePoint 2 " << oid << std::endl;

        ChildIndex* childNodeIndex = gp->pointMap[treeRef][oid];

        //TODO: Is pointMap[treeRef][oid] always initialized??
        if (childNodeIndex == NULL) {
            return false;
        }
        
        InternalNode* node = childNodeIndex->first;

        return node->deletePoint(oid, childNodeIndex->second, treeRef);
    }




    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::InternalNode::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                           RType* r,
                                                           int tickNum, 
                                                           int tickRef) const {

        LOG(DEBUG) << "InternalNode::enumerateOverRegion: Node = " << *this
                   << " Region = " << *r << std::endl;

//         if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
//             std::cout << "Comparing internal node " << *this << std::endl;
//         }

        int relativePosition = getRelativePosition(r, tickNum, tickRef);

        if (relativePosition == INSIDE) {



//             if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
//                 std::cout << "INSIDE" << std::endl;
//             }


            LOG(DEBUG) << "INSIDE" << std::endl;

            reportAll(cb);
        } 
        else if (relativePosition == OVERLAP) {
            LOG(DEBUG) << "OVERLAP" << std::endl;

//             if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
//                 std::cout << "OVERLAP" << std::endl;
//             }


            for (int i = 0; i < NUM_CHILDREN; i++) {
                if (childrenPtr[i] != NULL) {
                    if (isLeaf[i]) {
                        LeafNode* leaf = dynamic_cast<LeafNode*>(childrenPtr[i]);


//                         if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
//                             std::cout << "Checking leaf " << *leaf << std::endl;
//                         }




                        leaf->enumerateOverRegion(cb, r);
                    } 
                    else {
                        InternalNode* inode 
                            = dynamic_cast<InternalNode*>(childrenPtr[i]);



//                         if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
//                             std::cout << "Checking inode " << *inode << std::endl;
//                         }

                        inode->enumerateOverRegion(cb, r, tickNum, tickRef);
                    }
                }
                else {
//                     if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
//                         std::cout << "Child " << i << " is NULL." << std::endl;
//                     }

                }
                
            }
        }
        else {
//             if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
//                 std::cout << "DISJUNCT" << std::endl;
//             }

        }


        if(Stripes<U,K,A>::toPrint && Node::id == 18179) {
            Stripes<U,K,A>::toPrint = false;
        }

    }

   template<typename U, typename K, typename A>
   int Stripes<U,K,A>::InternalNode::deleteUnupdatedPoints(std::vector<U*>* unupdatedPoints, 
                                                           int treeRef) {
       int numDeleted = 0;
            
       for (int i = 0; i < NUM_CHILDREN; i++) {
           if (childrenPtr[i] != NULL) {
               if (isLeaf[i]) {
                   LeafNode* leaf = dynamic_cast<LeafNode*>(childrenPtr[i]);
                   numDeleted += leaf->size;
                        
                   for (int j = 0; j < leaf->size; j++) {
                       gp->pointMap[treeRef][K::getOID(leaf->entries[j])] = NULL;
                       unupdatedPoints->push_back(leaf->entries[j]);
                   }
                   delete leaf;
               } 
               else {
                   InternalNode* inode
                       = dynamic_cast<InternalNode*>(childrenPtr[i]);
                   numDeleted += inode->deleteUnupdatedPoints(unupdatedPoints, treeRef);
                   isLeaf[i] = true;
                   delete inode;
               }
               childrenPtr[i] = NULL;
           }
       }
       return numDeleted;
   }

   template<typename U, typename K, typename A>
   void Stripes<U,K,A>::InternalNode::drawQuads(FILE* gnuplot, bool xDim) {
        double SL_Px, SL_Vx;
        double SL_Py, SL_Vy;

        if(xDim) {
            SL_Px = (double)(gp->pxMax + 2 * gp->vxMax * gp->lifeTime) 
                / (1 << Node::level);

            SL_Vx = (double)(2 * gp->vxMax) / (1 << Node::level);

            double ll_px = Node::grid.px;
            double ll_vx = Node::grid.vx;

            double ur_px = ll_px + SL_Px;
            double ur_vx = ll_vx + SL_Vx;


            //fprintf(gp, "%f %f %f %f\n", ll_vx, ll_px, ur_vx, ur_px);
            fprintf(gnuplot, "set obj %d rect from %d,%d to %d, %d\n", 
                    (Node::id+1), (int)ll_vx, (int)ll_px, (int)ur_vx, (int)ur_px);

            // printf("set obj %d rect from %d,%d to %d, %d\n", 
            //        (Node::id+1), (int)ll_vx, (int)ll_px, (int)ur_vx, (int)ur_px);

            //std::cout << ll_vx  << " " <<  ll_px << " " << ur_vx << " " << ur_px << std::endl;
        }
        else {
            SL_Py = (double)(gp->pyMax + 2 * gp->vyMax * gp->lifeTime) 
                / (1 << Node::level);
            SL_Vy = (double)(2 * gp->vyMax) / (1 << Node::level);


            double ll_py = Node::grid.py;
            double ll_vy = Node::grid.vy;

            double ur_py = ll_py + SL_Py;
            double ur_vy = ll_vy + SL_Vy;

            //fprintf(gp, "%f %f %f %f\n", ll_vy, ll_py, ur_vy, ur_py);
            fprintf(gnuplot, "set obj %d rect from %d,%d to %d, %d\n", 
                    (Node::id + 1), (int)ll_vy, (int)ll_py, (int)ur_vy, (int)ur_py);
            
            // printf("set obj %d rect from %d,%d to %d, %d\n", 
            //        (Node::id + 1), (int)ll_vy, (int)ll_py, (int)ur_vy, (int)ur_py);

            //std::cout << ll_vy  << " " <<  ll_py << " " << ur_vy << " " << ur_py << std::endl;
        }

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (childrenPtr[i] != NULL) {
                if (isLeaf[i]) {
                    LeafNode* leaf = dynamic_cast<LeafNode*>(childrenPtr[i]);

                    double SL_P = (double)(gp->pyMax + 2 * gp->vyMax * gp->lifeTime) 
                        / (1 << (Node::level + 1));

                    double SL_V = (double)(2 * gp->vyMax) / (1 << (Node::level + 1));
                    //leaf->printQuads(gp, xDim, SL_P, SL_V);
                    
                } 
                else {
                    InternalNode* inode
                        = dynamic_cast<InternalNode*>(childrenPtr[i]);
                    inode->drawQuads(gnuplot, xDim); 
                }
            }
        }
    }

    // template<typename U, typename K, typename A>
    // std::ostream& operator<<(std::ostream &out, 
    //                          const typename Stripes<U,K,A>::InternalNode& node) {

    //     out << "InternalNode(" << std::setiosflags(std::ios::fixed)
    //         << std::setprecision(2) << node.nid << ", " << node.level << ", "
    //         << "{(" << node.grid.vx << "," << node.grid.px << "), (" 
    //         << node.grid.vy << "," << node.grid.py << ")}"
    //         << ", " << node.size << ")";
    // }
    //------------------------------------------------------------------------
    // Private Methods
    //------------------------------------------------------------------------



    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::deleteSubtree(InternalNode* node) {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (node->childrenPtr[i] != NULL) {
                if (node->isLeaf[i]) {
                    delete node->childrenPtr[i];
                } else {
                    InternalNode* iNode 
                        = dynamic_cast<InternalNode*>(node->childrenPtr[i]);
                    deleteSubtree(iNode);
                }
            }
        }
        // TODO: Does this delete internal node correctly?
        delete node;
    }




    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::countSubTree(InternalNode* node) const {
        if(node == NULL) 
            return 0;
        
        int count = 0;
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (node->childrenPtr[i] != NULL) {
                if (node->isLeaf[i]) {
                    LeafNode* ln = dynamic_cast<LeafNode*>(node->childrenPtr[i]);
                    count += ln->size;
                } else {
                    InternalNode* iNode
                        = dynamic_cast<InternalNode*>(node->childrenPtr[i]);
                    count += countSubTree(iNode);
                }
            }
        }
        return count;
    }



    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::printSubTree(InternalNode* node) const {
        if (node == NULL) return;

        std::cout << "\t" << *node << std::endl;
        std::vector<InternalNode*> nodes;

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (node->childrenPtr[i] != NULL) {
                if (node->isLeaf[i]) {
                    LeafNode* ln = dynamic_cast<LeafNode*>(node->childrenPtr[i]);

                    std::cout << "\t\t" << i << " :Leaf : " << *ln;
                    std::cout << std::endl;
                } else {
                    InternalNode* iNode
                        = dynamic_cast<InternalNode*>(node->childrenPtr[i]);


                    std::cout << "\t\t" << i << " :INode: " << *iNode << std::endl;
                    nodes.push_back(iNode);
                }
            }
            else {
                std::cout << "\t\tNULL" << std::endl;
            }
        }
        LOG(DEBUG) << "\t\t---------------------------------------" << std::endl;
        typename std::vector<InternalNode*>::const_iterator iter;
        for(iter = nodes.begin(); iter != nodes.end(); iter++) {
            printSubTree(*iter);
        }

    }

    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::countInternalNodes(InternalNode* node) const {
        int count = 1;

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (node->childrenPtr[i] != NULL && !node->isLeaf[i]) {
                InternalNode* inode;
                inode = dynamic_cast<InternalNode*>(node->childrenPtr[i]);
                count += countInternalNodes(inode);
            }
        }
        return count;
    }

    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::countLeafNodes(InternalNode* node) const {
        int count = 0;

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (node->childrenPtr[i] != NULL && node->isLeaf[i]) {
                count += 1;
            }
            else if(!node->isLeaf[i]){
                InternalNode* inode;
                inode = dynamic_cast<InternalNode*>(node->childrenPtr[i]);
                count += countLeafNodes(inode);
            }
        }
        return count;
    }

    template<typename U, typename K, typename A>
    int Stripes<U,K,A>::computeHeight(InternalNode* node) const {
        int nextLevelMax = 0;
        for(int i = 0; i < NUM_CHILDREN; i++) {
            if (node->childrenPtr[i] != NULL) {

                int currHeight = 0;

                if(node->isLeaf[i]) {
                    currHeight = 1;
                }

                else {
                    InternalNode* inode;
                    inode = dynamic_cast<InternalNode*>(node->childrenPtr[i]);
                    currHeight = computeHeight(inode);
                }
                if(currHeight > nextLevelMax) {
                    nextLevelMax = currHeight;
                }
            }
        }
        return nextLevelMax + 1;
    }


    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------

   template<typename U, typename K, typename A>
   bool Stripes<U,K,A>::toPrint = false;

   template<typename U, typename K, typename A>
   int Stripes<U,K,A>::plotTick = 49;

    template<typename U, typename K, typename A>
    Stripes<U,K,A>::Stripes(int vxMax, int vyMax, int pxMax, 
                            int pyMax, int lifeTime) {

        //tmp
   
        // Make the lifeTime part of the name. 
        sprintf(name, "Stripes%d", lifeTime);


        tickNum = 0;
        
        gp = new GlobalParams(vxMax, vyMax, pxMax, pyMax, lifeTime, pointMap);

    }

    template<typename U, typename K, typename A>
    Stripes<U,K,A>::~Stripes() {
        delete gp;
    }



    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::startTick(std::vector<U*>* agents) {
        plotCount = 1;

        //TMP copy points. 
        this->agentsCopy = new std::vector<U*>(*agents);

        // Very first tick. Initialize and construct the first tree. 
        if(tickNum == 0) {
            tree[0] = new InternalNode(gp);
            tree[1] = new InternalNode(gp);
            tickRef[0] = 0;
            tickRef[1] = 0;

            for (int i = 0; i < agents->size(); i++) {
                U* point = (*agents)[i];
                tree[1]->insertPoint(point, 1, tickNum, tickRef[1]);
            }
        }

        //std::cout << "tick: " << tickNum<< std::endl;
        // std::cout << countSubTree(tree[0]) << " " << countSubTree(tree[1]) << " ";
        // std::cout << gp->pointMap[0].size() << " " << gp->pointMap[1].size() << std::endl;        

        

//         if(tickNum == 4) {
//             int count;
        // std::cout << "tree 0..." << std::endl;
        // printSubTree(tree[0]);

        // std::cout << "tree 1..." << std::endl;
        // printSubTree(tree[1]);
//         }

        // std::cout << "At end of tick " << tickNum << std::endl;
        // std::cout << "tree 0..." << std::endl;
        // printSubTree(tree[0], count);

        // std::cout << "tree 1..." << std::endl;
        // printSubTree(tree[1], count);

    }


    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {

/*         if(tickNum == plotTick && plotCount > 0) { */
/*             RType* r2 = new RType(r->getLow0(), r->getHigh0(),  */
/*                                   r->getLow1(), r->getHigh1()); */
/*             queriesToPlot.push_back(r2); */
/*             plotCount--; */
/*         } */
/*         else if(tickNum == plotTick && plotCount == 0) { */
/*             drawTree(0, true, agentsCopy, "tree0_xdim_quads.eps", &queriesToPlot); */
/*             plotCount--; */
/*         } */

        // Query and report matches from both trees. 
        for(int i = 0; i < 2; i++) {
            //std::cout << "Checking tree " << i << std::endl;
            tree[i]->enumerateOverRegion(cb, r, tickNum, tickRef[i]);
        }
    }


    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::afterInsert(U* u) {

        // if(tickNum == 10) {
        //std::cout << "In after insert " << *u << std::endl;
        // }
        // Decide which tree gets updates/inserts this tick. 
        int index = (tickNum % (2 * gp->lifeTime) < gp->lifeTime) ? 0 : 1;

        tree[index]->insertPoint(u, index, tickNum+1, tickRef[index]);

    }

    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::beforeDelete(U* u) {

        // if(tickNum == 10) {
        //std::cout << "In before delete " << *u << std::endl;
        // }


        // First try to delete the point from tree 0. 
        if (!(tree[0]->deletePoint(K::getOID(u), 0))) { 
            // If it was not found, delete the point from tree 1. 
            if(!tree[1]->deletePoint(K::getOID(u), 1)) {
                //std::cout << "Failed to delete " << *u << std::endl;
            }
        }
    }

    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::beforeUpdateVelocity(U* point,
                                              typename K::KeyType0 new0,
                                              typename K::KeyType1 new1,
                                              typename K::KeyType0 newV0,
                                              typename K::KeyType1 newV1) {


        //std::cout << "In before update velocity " << *point << std::endl;


        // Delete existing point from the index. 
        beforeDelete(point);

        // Decide which tree gets updates this tick. 
        int index = (tickNum % (2 * gp->lifeTime) < gp->lifeTime) ? 0 : 1;


        // Compute transformed point with new position and velocity. 
        double transVx = newV0 + gp->vxMax;
        double transVy = newV1 + gp->vyMax;

        double pxRef = new0
            - (transVx - gp->vxMax) * (tickNum - tickRef[index] + 1)
            + gp->vxMax * gp->lifeTime;


        double pyRef = new1 
            - (transVy - gp->vyMax) * (tickNum - tickRef[index] + 1)
            +  gp->vyMax * gp->lifeTime;


        TransformedPoint* tp = new TransformedPoint(transVx, transVy, 
                                                    pxRef, pyRef);

        tree[index]->insertPoint(point, tp, index, tickNum+1, tickRef[index]);

    }

    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::endTick(bool force) {

        delete agentsCopy;

        if (force) {
            gp->pointMap[0].clear();
            gp->pointMap[1].clear();

            if (tree[0] != NULL) {
                deleteSubtree(tree[0]);
            }

            if (tree[0] != NULL) {
                deleteSubtree(tree[1]);
            }

            Node::resetId();
            tickNum = 0;
            tickRef[0] = tickRef[1] = 0;

            return;
        }

        tickNum++;
        int modl = (tickNum) % gp->lifeTime;
        int mod2l = (tickNum) % (2*gp->lifeTime);


        if(modl == 0) {
            int delIndex;

            // At tick 2k * lifetime. Clear out idx0 and move to idx1.
            if(mod2l == 0) {
                delIndex = 0;
            }
            // At tick (2k+1) * lifetime. Clear out idx1 and move to idx0. 
            else {
                DBUTL_ASSERT((mod2l == gp->lifeTime));
                delIndex = 1;
            }

            std::vector<U*> unupdatedPoints;
            int numDeleted
                = tree[delIndex]->deleteUnupdatedPoints(&unupdatedPoints, 
                                                        delIndex);
            tree[delIndex]->size -= numDeleted;
                
            DBUTL_ASSERT((tree[delIndex]->size == 0));

            tickRef[delIndex] = tickNum;

            for (int i = 0; i < unupdatedPoints.size(); i++) {
                U* point = unupdatedPoints[i];

//                 if(K::getOID(point) == 6071) {
//                     std::cout << "Unupdated!! " << *point << std::endl;
//                 }


                tree[1 - delIndex]->insertPoint(point, 
                                                1 - delIndex, 
                                                tickNum, 
                                                tickRef[1 - delIndex]);
            }


            // std::cout << "Updating index " << delIndex 
            //           << " refTime to " << tickNum << std::endl;
        }
        
    }




    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::drawQueries(FILE* gnuplot, int index, 
                                     bool xDim, std::vector<RType*>* queries) {





        for(int i = 0; i < queries->size(); i++) {
            RType* r = queries->at(i);

            double ll, ul, lr, ur;
        
            if(xDim) {
                ll = r->getLow0() + gp->vxMax * (tickNum - tickRef[index] + gp->lifeTime);
                ul = r->getHigh0() + gp->vxMax * (tickNum - tickRef[index] + gp->lifeTime);
                lr = r->getLow0() + gp->vxMax * (gp->lifeTime - tickNum + tickRef[index]);
                ur = r->getHigh0() + gp->vxMax * (gp->lifeTime - tickNum + tickRef[index]); 

                fprintf(gnuplot, "set obj %d polygon from %d,%d to %d,%d to %d,%d to %d,%d to %d,%d\n", 
                        10000000 + i, 
                        (int)0, (int)ll, 
                        (int)0, (int)ul,                 
                        (int)(2 * gp->vxMax), (int)ur,
                        (int)(2 * gp->vxMax), (int)lr, 
                        (int)0, (int)ll);

                printf("set obj %d polygon from %d,%d to %d,%d to %d,%d to %d,%d to %d,%d\n", 
                        10000000 + i, 
                        (int)0, (int)ll, 
                        (int)0, (int)ul,                 
                        (int)(2 * gp->vxMax), (int)ur,
                        (int)(2 * gp->vxMax), (int)lr, 
                        (int)0, (int)ll);
                

            }
            else {
                ll = r->getLow1() + gp->vyMax * (tickNum - tickRef[index] + gp->lifeTime);
                ul = r->getHigh1() + gp->vyMax * (tickNum - tickRef[index] + gp->lifeTime);
                lr = r->getLow1() + gp->vyMax * (gp->lifeTime - tickNum + tickRef[index]);
                ur = r->getHigh1() + gp->vyMax * (gp->lifeTime - tickNum + tickRef[index]); 

                fprintf(gnuplot, "set obj %d polygon from %d,%d to %d,%d to %d,%d to %d,%d\n", 
                        10000000 + i, 
                        (int)0, (int)ll, 
                        (int)0, (int)ul,                 
                        (int)(2 * gp->vyMax), (int)ur,
                        (int)(2 * gp->vyMax), (int)lr,
                        (int)0, (int)ll);
            }

            fprintf(gnuplot, "set obj %d front fs empty border rgb \"red\" lw 3\n", 10000000 + i);

        }
    }


    template<typename U, typename K, typename A>
    void Stripes<U,K,A>::drawTree(int index, 
                                  bool xDim, 
                                  std::vector<U*>* agents, 
                                  const char* name, 
                                  std::vector<RType*>* queries) {

        FILE* gnuplot = popen("gnuplot", "w"); 

        fprintf(gnuplot, "set terminal postscript eps enhanced color;\n");
        fprintf(gnuplot, "set output '%s';\n", name);
        fprintf(gnuplot, "unset key;\n");

        // printf("set terminal postscript eps enhanced color;\n");
        // printf("set output '%s';\n", name);
        // printf("unset key;\n");


        //fprintf(gp, "set view map;");
        //fprintf(gp, "set style data lines;");
        //fprintf(gp, "set title 'Point Heat Map';");
        //fprintf(gp, "set cblabel 'Score';");
        //fprintf(gp, "set palette rgbformulae -7, 2, -7;");



        // double vxMax = gp->vxMax;
        // double pxMax = OptimizedStripesINode<U, K>::pxMax;
        
        // double vyMax = OptimizedStripesINode<U, K>::vyMax;
        // double pyMax = OptimizedStripesINode<U, K>::pyMax;

        // int lt = OptimizedStripesINode<U, K>::LIFE_TIME;

        double space_Px = (double)(gp->pxMax + 2 * gp->vxMax * gp->lifeTime);
        double space_Vx = (double)(2 * gp->vxMax);

        double space_Py = (double)(gp->pyMax + 2 * gp->vyMax * gp->lifeTime) / 1;
        double space_Vy = (double)(2 * gp->vyMax) / 1;
            
        //fprintf(gp, "plot '-' using 1:2:($3-$1):($4-$2) notitle with vectors\n");
            
        tree[index]->drawQuads(gnuplot, xDim);
        std::cout << "HERE" << std::endl;

        if(queries != NULL) {
            drawQueries(gnuplot, index, xDim, queries);
        }

        if(xDim) {
            fprintf(gnuplot, "plot [0:%d][0:%d] '-' using 1:2 notitle lc rgb \"green\"\n", 
                    (int)space_Vx, (int)space_Px);

            // fprintf(gnuplot, "plot [750:1300][1750:13000] '-' using 1:2 notitle lc rgb \"green\"\n", 
            //          /*(int)space_Vx, */(int)space_Px);

        }
        else {
            fprintf(gnuplot, "plot [0:%d][0:%d] '-' using 3:4 notitle lc rgb \"green\"\n", 
                    (int)space_Vy, (int)space_Py);
        }

        
        for(int i = 0; i < agents->size(); i++) {
            U* u = agents->at(i);

            double transVx = K::getV0(u) + gp->vxMax;
            double transVy = K::getV1(u) + gp->vyMax;
            double pxRef = K::getKey0(u) 
                - (transVx - gp->vxMax) * (tickNum - tickRef[index]) + gp->vxMax * gp->lifeTime;

            double pyRef = K::getKey1(u) 
                - (transVy - gp->vyMax) * (tickNum - tickRef[index]) + gp->vyMax * gp->lifeTime;

            fprintf(gnuplot, "%lf %lf %lf %lf\n", transVx, pxRef, transVy, pxRef);
        }

        fprintf(gnuplot,"e\n");
        pclose(gnuplot);

    }
}





#endif

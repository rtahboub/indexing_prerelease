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


#ifndef _TPRTREE_H_
#define _TPRTREE_H_

#include <map>
#include <set>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
//#include <tr1/unordered_map>
#include <unordered_map>

#include "2D/MovingSpatialIndex2D.h"
#include "2D/point_index/TPRegion.h"

//#define REBUILD_EVERY_TICK 1

//#define BULK_LOAD 1
#define DELETE_BY_HASH 1
#define USE_TPR_EPS 1
#define USE_NODE_ID 1
//#define OUTPUT_QMEASURE 1

namespace PSimIndex {

    // //Max entries per node.    
    // const unsigned int TPR_M = 16;

    // //Min entries per node. 
    // const unsigned int TPR_m = 1;

#ifdef USE_TPR_EPS
    const unsigned int TPR_PRECISION = 4; // used in debug to determine how many digits to output
    const double TPR_EPS = 1.e-9;
#endif

    //sowell: hashMap can't be static. Means every node has to have pointer... Bummer. 


    template<typename U, typename K, int TPR_m, int TPR_M>
    class TPRTreeNode2D;

    template<typename U, typename K, int TPR_m, int TPR_M>
    struct NodeIndex {
        TPRTreeNode2D<U, K, TPR_m, TPR_M>* parent;
        int index;

        NodeIndex() {
            parent = NULL;
            index = 0;
        }

        NodeIndex(TPRTreeNode2D<U, K, TPR_m, TPR_M>* p, int idx) {
            parent = p;
            index = idx;
        }
    }; // NodeIndex



    template<typename U, typename K, int TPR_m, int TPR_M>
    class TPRTreeNode2D {
    public:
    // template<typename U1, typename K1, typename A1, typename RG1, int TPR_m1, int TPR_M1>
    // friend class AENEJoin;



        static int TPR_W, TPR_U, TPR_H;
        static bool TPR_Use_Naive_Split;


#ifdef DELETE_BY_HASH
        // for hash look-up in 'updates'
        //TODO: Should be private?


       // typedef std::tr1::unordered_map<typename K::IDType, NodeIndex<U,K,TPR_m,TPR_M>*> HashType;
        typedef std::unordered_map<typename K::IDType, NodeIndex<U,K,TPR_m,TPR_M>*> HashType;


//        static HashType hashMap; 
#endif


        //private:
        typedef TPRegion<typename K::KeyType0, typename K::KeyType1> TPRType;
        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
        //HERE
        union Ptr {
            TPRTreeNode2D<U, K, TPR_m, TPR_M>* childPtr; 
            U* data; 
        };
        
        // parent pointer is recovered..
        TPRTreeNode2D<U, K, TPR_m, TPR_M>* parent; 

#ifdef DELETE_BY_HASH        
        HashType* hashMap;
#endif

        unsigned int level, index;

        TPRType mbrs[ TPR_M ];
        Ptr ptrs[ TPR_M ];


    public:


#ifdef USE_NODE_ID
        int eid[ TPR_M ];
        int id;
        static int nextID;
#endif


        
        //TODO: Make private?
        unsigned int count;

        inline void setMBR(int i, TPRType mbr) {
            mbrs[i] = mbr;
        }

        inline void setPtr(int i, Ptr ptr) {
            ptrs[i] = ptr;
        }

        inline void setLevel(unsigned int level) {
            this->level = level;
        }

        inline unsigned int getLevel() const {
            return level;
        }

        inline void setIndex(unsigned int index) {
            this->index = index;
        }

        inline unsigned int getIndex() const {
            return index;
        }
        
        inline unsigned int getCount() const{
            return count;
        }


        inline void setParent(TPRTreeNode2D<U, K, TPR_m, TPR_M>* parent) {
            this->parent = parent;
        }

        inline TPRTreeNode2D<U, K, TPR_m, TPR_M>* getParent() const {
            return parent;
        }
        


        TPRTreeNode2D(HashType* hash,/*int TPR_H = 60*/int level = 0) {
            count = 0;
            this->level = level;
            this->hashMap = hash;
            parent = NULL;
            index = 0;
            //this->TPR_H = TPR_H;
#ifdef USE_NODE_ID
            id = nextID++;
#endif
        }

        ~TPRTreeNode2D() {
        }

        bool isLeaf() const { return level == 0; }

        bool isRoot() const { return parent == NULL; }

        bool isInternalNode() const { return level > 0; }

        //TODO: delete me. 
        void setNewID() {
#ifdef USE_NODE_ID
            id = nextID++;
#endif
        }


//sowell: Why is any of this in the Node class?
//#ifdef BULK_LOAD
        /**
         */
        class entryComp {
        private:

            //TPRType* _mbrs;
            const std::vector<TPRType>& _mbrs;
            //Ptr* _ptrs;
            //const std::vector<TPRTreeEntry2D>& _entries;
            const int _sort_axis;

        public:

            entryComp(const std::vector<TPRType>& mbrs,
                      //Ptr* ptrs,
                      int sort_axis)
                : _mbrs(mbrs), /*ptrs(ptrs),*/ _sort_axis(sort_axis) 
            {}

            /* For given axis, sort the entries
             * by the lower then by the upper value
             */
            bool operator()(int a, int b) const {
                if(_sort_axis == 0) {// sort by x
                    return ((_mbrs[a].getHigh0P() + _mbrs[a].getLow0P())
                            < (_mbrs[b].getHigh0P() + _mbrs[b].getLow0P()));
                    //return (_entries.at(a).mbr.getHigh0P() + _entries.at(a).mbr.getLow0P()) < (_entries.at(b).mbr.getHigh0P() + _entries.at(b).mbr.getLow0P());
                }
                else if(_sort_axis == 1) {// sort by y
                    return ((_mbrs[a].getHigh1P() + _mbrs[a].getLow1P())
                            < (_mbrs[b].getHigh1P() + _mbrs[b].getLow1P()));
                    //return (_entries.at(a).mbr.getHigh1P() + _entries.at(a).mbr.getLow1P()) < (_entries.at(b).mbr.getHigh1P() + _entries.at(b).mbr.getLow1P());
                }
                else if(_sort_axis == 2) {// sort by vx
                    return ((_mbrs[a].getHigh0V() + _mbrs[a].getLow0V())
                            < (_mbrs[b].getHigh0V() + _mbrs[b].getLow0V()));
                    // return (_entries.at(a).mbr.getHigh0V() + _entries.at(a).mbr.getLow0V()) < (_entries.at(b).mbr.getHigh0V() + _entries.at(b).mbr.getLow0V());
                }
                else {// sort by vy
                    return ((_mbrs[a].getHigh1V() + _mbrs[a].getLow1V())
                            < (_mbrs[b].getHigh1V() + _mbrs[b].getLow1V()));
                    // return (_entries.at(a).mbr.getHigh1V() + _entries.at(a).mbr.getLow1V()) < (_entries.at(b).mbr.getHigh1V() + _entries.at(b).mbr.getLow1V());
                }
            }
        };

        
        void orderData(int dim, 
                       int numNodes, 
                       //TPRType* mbrs,
                       std::vector<TPRType> _mbrs,
                       //std::vector<TPRTreeEntry2D>& data, 
                       std::vector<int>& idx, 
                       int from, 
                       int to, 
                       int Sx, 
                       int Sy, 
                       int Vx, 
                       int Vy) {
            /* dimension mapping
                [dim, S/V]
                [4,   Vy ]
                [3,   Vx ]
                [2,   Sy ]
                [1,   Sx ]
            */
            
            if(from == to) return; // no data to be sorted

            // compute the number of slabs
            int numSlabs;
            if(dim == 4) numSlabs = Vy;
            else if(dim == 3) numSlabs = Vx;
            else if(dim == 2) numSlabs = Sy;
            else numSlabs = Sx;

            DBUTL_ASSERT(to-from >= numSlabs);

            // sort the data
#ifdef OUTPUT_QMEASURE
            LOG(INFO) << "[TPR, HBU] sorting from " << from << " to " << to << std::endl;
#endif

            std::sort(& (idx[from]), &(idx[to]), entryComp(_mbrs, dim - 1));
            
#ifdef OUTPUT_QMEASURE
            LOG(INFO) << "[TPR, HBU] sorted by dimension " << dim << std::endl;
#endif

            // check if we are done sorting
            if(dim == 1) return;



            
            if(numSlabs < 1) numSlabs = 1;
            if(numSlabs > numNodes) numSlabs = numNodes;

            // recursive calls
            int nodesPerSlab = (numNodes + numSlabs - 1) / numSlabs;
            for(int i=0;i<numSlabs;++i) {
                orderData(dim-1, nodesPerSlab, _mbrs, idx, from, 
                          std::min(to, from+TPR_M*nodesPerSlab), 
                          Sx, Sy, Vx, Vy); 
            }
        }

        /*
        this determines the product of Sx,Vx and Sy,Vy suggested by the HBU algorithm.
        *
        * sowell: Never gets called...
        */
        // void determineSV(int& SVx, int& SVy, std::vector<TPRTreeEntry2D>* data, int k) {
        //     DBUTL_ASSERT (data->size() > 0);
        //     double mean_sx, stdev_sx;
        //     double mean_sy, stdev_sy;
        //     double mean_vx, stdev_vx;
        //     double mean_vy, stdev_vy;

        //     mean_sx = mean_sy = 0.0;
        //     mean_vx = mean_vy = 0.0;

        //     for(int i=0;i<data->size();++i) {
        //         mean_sx += ((*data)[i].mbr.getLow0P() + (*data)[i].mbr.getHigh0P()) / 2.;
        //         mean_sy += ((*data)[i].mbr.getLow1P() + (*data)[i].mbr.getHigh1P()) / 2.;
        //         mean_vx += ((*data)[i].mbr.getLow0V() + (*data)[i].mbr.getHigh0V()) / 2.;
        //         mean_vy += ((*data)[i].mbr.getLow1V() + (*data)[i].mbr.getHigh1V()) / 2.;
        //     }

        //     mean_sx /= (double)(data->size());
        //     mean_sy /= (double)(data->size());
        //     mean_vx /= (double)(data->size());
        //     mean_vy /= (double)(data->size());

        //     stdev_sx = stdev_sy = 0.0;
        //     stdev_vx = stdev_vy = 0.0;

        //     for(int i=0;i<data->size();++i) {
        //         double sx, sy, vx, vy;
        //         sx = ((*data)[i].mbr.getLow0P() + (*data)[i].mbr.getHigh0P()) / 2.;
        //         sy = ((*data)[i].mbr.getLow1P() + (*data)[i].mbr.getHigh1P()) / 2.;
        //         vx = ((*data)[i].mbr.getLow0V() + (*data)[i].mbr.getHigh0V()) / 2.;
        //         vy = ((*data)[i].mbr.getLow1V() + (*data)[i].mbr.getHigh1V()) / 2.;

        //         stdev_sx += pow((mean_sx - sx), 2);
        //         stdev_sy += pow((mean_sy - sy), 2);
        //         stdev_vx += pow((mean_vx - vx), 2);
        //         stdev_vy += pow((mean_vy - vy), 2);
        //     }

        //     if(stdev_sy * stdev_vy < 1.e-10) {
        //         SVy = SVx = (int) sqrt(k);

        //         return;            
        //     }
        //     double B = stdev_sx * stdev_vx / (stdev_sy * stdev_vy);
        //     B = sqrt(B);

        //     SVx = (int) sqrt(B * k);
        //     SVy = (int) sqrt(k / B);
        //     if(SVx < 1) SVx = 1;
        //     if(SVy < 1) SVy = 1;
        //     if(SVx > k) SVx = k;
        //     if(SVy > k) SVy = k;

        // }


        void bulkload(std::vector<U*>* agents, int tickNum) {
            std::vector<TPRType> _mbrs;
            std::vector<Ptr> _ptrs;
            Ptr ptr;
            
            // leaf nodes
            int num_points = agents->size();
            
            for(int i = 0 ; i < num_points ; ++i) {
                U* u = (*agents)[i];
                RType pos_r(K::getKey0(u), K::getKey0(u),
                            K::getKey1(u), K::getKey1(u));
                RType vel_r(K::getV0(u), K::getV0(u),
                            K::getV1(u), K::getV1(u));
                TPRType mbr(&pos_r, &vel_r, tickNum);
                ptr.data = agents->at(i);

                _mbrs.push_back(mbr);
                _ptrs.push_back(ptr);

                // TPRTreeEntry2D newEntry(mbr, (*agents)[i]);
                // data.push_back(newEntry);
            }

            bulkloadInner(_mbrs, _ptrs, tickNum);
        }

        template<typename RG>
        void bulkloadQueries(std::vector<U*>* agents, int tickNum, RG* rg) {
            std::vector<TPRType> _mbrs;
            std::vector<Ptr> _ptrs;
            Ptr ptr;
            
            // leaf nodes
            int num_points = agents->size();

            //std::cout << "num_points: " << num_points << std::endl;
            
            for(int i = 0 ; i < num_points ; ++i) {
                U* u = (*agents)[i];
                RType pos_r = rg->getRegion(u);

                //std::cout << "bulk loading queries: " << pos_r << std::endl;

                // RType pos_r(K::getKey0(u), K::getKey0(u),
                //             K::getKey1(u), K::getKey1(u));
                RType vel_r(K::getV0(u), K::getV0(u),
                            K::getV1(u), K::getV1(u));
                TPRType mbr(&pos_r, &vel_r, tickNum);
                ptr.data = agents->at(i);

                _mbrs.push_back(mbr);
                _ptrs.push_back(ptr);

                // TPRTreeEntry2D newEntry(mbr, (*agents)[i]);
                // data.push_back(newEntry);
            }

            bulkloadInner(_mbrs, _ptrs, tickNum);
        }


        void bulkloadInner(std::vector<TPRType>& _mbrs, std::vector<Ptr>& _ptrs, int tickNum) {

            int level = 0; // leaf level = 0
            int num_points, num_nodes;
            std::vector<TPRTreeNode2D*> nodes;
            std::vector<TPRTreeNode2D*> children;
            
            int lcount = 0;

            do {
                // prepare the data
                //std::vector<TPRTreeEntry2D> data;
                //std::vector<TPRType> _mbrs;
                //std::vector<Ptr> _ptrs;
                Ptr ptr;

                if(level == 0) {
                    num_points = _mbrs.size();
                }
                //data.clear();

                // if(level == 0) {
                //     // leaf nodes
                //     num_points = agents->size();
                    
                //     for(int i = 0 ; i < num_points ; ++i) {
                //         U* u = (*agents)[i];
                //         RType pos_r(K::getKey0(u), K::getKey0(u),
                //                     K::getKey1(u), K::getKey1(u));
                //         RType vel_r(K::getV0(u), K::getV0(u),
                //                     K::getV1(u), K::getV1(u));
                //         TPRType mbr(&pos_r, &vel_r, tickNum);
                //         ptr.data = agents->at(i);

                //         _mbrs.push_back(mbr);
                //         _ptrs.push_back(ptr);

                //         // TPRTreeEntry2D newEntry(mbr, (*agents)[i]);
                //         // data.push_back(newEntry);
                //     }
                // }
                else {
                    _mbrs.clear();
                    _ptrs.clear();


                    // internal nodes
                    num_points = children.size();

                    for(int i = 0 ; i < num_points ; ++i) {
                        //TPRTreeNode2D* childPtr = children[i];

                        ptr.childPtr = children[i];
                        TPRType mbr = ptr.childPtr->getCover(tickNum);
                        _ptrs.push_back(ptr);
                        _mbrs.push_back(mbr);

                        // TPRTreeEntry2D newEntry(mbr, childPtr);
                        // data.push_back(newEntry);
                    }
                }
                
                // compute the number of nodes at each level:
                num_nodes = (num_points + TPR_M - 1) / TPR_M;

                // Determine S,V for each dimension
                
                int SVx, SVy;
                
                //determineSV(SVx, SVy, &data, num_nodes);
                SVx = SVy = (int) sqrt(num_nodes);                
                while(SVx * SVy < num_nodes) {
                    if((SVx+1)*SVy >= num_nodes) {
                        SVx++;
                        break;
                    }
                    if(SVx*(SVy+1) >= num_nodes) {
                        SVy++;
                        break;
                    }
                    if(SVx * SVy < num_nodes) {
                        SVx++;
                        SVy++;    
                    }
                }
                
                // use 1-D algorithm in each dimension to determine slabs
                int Sx, Sy, Vx, Vy;
                { // x coordinate (SVx = Sx * Vx)
                    double min_cost = 1.e10;
                    //double Hs[num_points], Hv[num_points];
                   
                    double* Hs = new double[num_points];
                    double* Hv = new double[num_points];

                    for(int i=0;i<num_points;++i) {
                        // Hs[i] = ( data[i].mbr.getLow0P() + data[i].mbr.getHigh0P()) / 2.;
                        // Hv[i] = ( data[i].mbr.getLow0V() + data[i].mbr.getHigh0V()) / 2.;

                        Hs[i] = ( _mbrs[i].getLow0P() + _mbrs[i].getHigh0P()) / 2.;
                        Hv[i] = ( _mbrs[i].getLow0V() + _mbrs[i].getHigh0V()) / 2.;
                    }
                    std::sort(Hs, Hs+num_points);
                    std::sort(Hv, Hv+num_points);
                    Sx = 1, Vx = SVx;
                    for(int s = 1; s<= SVx; ++ s) {
                        int v = (SVx + s - 1) / s;
                        double Ls[s], Lv[v];

                        {
                            int numPerSlabs = (num_points + s - 1) / s;
                            int pt = 0, end = 0;
                            for(int n = 0; n < s; ++n) {
                                end = std::min(num_points, pt + numPerSlabs);
                                // [pt, end)
                                Ls[n] = Hs[end-1] - Hs[pt];
                                pt += numPerSlabs;
                            }
                        }
                        {
                            int numPerSlabs = (num_points + v - 1) / v;
                            int pt = 0, end = 0;
                            for(int n = 0; n < v; ++n) {
                                end = std::min(num_points, pt + numPerSlabs);
                                // [pt, end)
                                Lv[n] = Hv[end-1] - Hv[pt];
                                pt += numPerSlabs;
                            }
                        }

                        double cost = 0.0;
                        double H = (double) TPR_H;
                        for(int i = 0; i < s ; ++i) {
                            for(int j = 0 ; j < v ; ++j) {
                                cost += Ls[i]*H + Lv[j]*H*H/2.;
                            }
                        }
                        
                        if(s == 1 || cost < min_cost) {
                            min_cost = cost;
                            Sx = s;
                            Vx = (SVx+s-1)/s;
                        }
                    }                    
                    delete [] Hs;
                    delete [] Hv;
                    
                }
                { // y coordinate
                    double min_cost = 1.e10;

                    //double Hs[num_points], Hv[num_points];

                    double* Hs = new double[num_points];
                    double* Hv = new double[num_points];

                    for(int i=0;i<num_points;++i) {
                        // Hs[i] = ( data[i].mbr.getLow1P() + data[i].mbr.getHigh1P()) / 2.;
                        // Hv[i] = ( data[i].mbr.getLow1V() + data[i].mbr.getHigh1V()) / 2.;

                        Hs[i] = ( _mbrs[i].getLow1P() + _mbrs[i].getHigh1P()) / 2.;
                        Hv[i] = ( _mbrs[i].getLow1V() + _mbrs[i].getHigh1V()) / 2.;
                    }
                    std::sort(Hs, Hs + num_points);
                    std::sort(Hv, Hv + num_points);
                    Sy = 1, Vy = SVy;
                    for(int s = 1; s<= SVy; ++ s) {
                        int v = (SVy + s - 1) / s;
                        double Ls[s], Lv[v];

                        {
                            int numPerSlabs = (num_points + s - 1) / s;
                            int pt = 0, end = 0;
                            for(int n = 0; n < s; ++n) {
                                end = std::min(num_points, pt + numPerSlabs);
                                // [pt, end)
                                Ls[n] = Hs[end-1] - Hs[pt];
                                pt += numPerSlabs;
                            }
                        }
                        {
                            int numPerSlabs = (num_points + v - 1) / v;
                            int pt = 0, end = 0;
                            for(int n = 0; n < v; ++n) {
                                end = std::min(num_points, pt + numPerSlabs);
                                // [pt, end)
                                Lv[n] = Hv[end-1] - Hv[pt];
                                pt += numPerSlabs;
                            }
                        }

                        double cost = 0.0;
                        double H = (double) TPR_H;
                        for(int i = 0; i < s ; ++i) {
                            for(int j = 0 ; j < v ; ++j) {
                                cost += Ls[i]*H + Lv[j]*H*H/2.;
                            }
                        }
                        
                        if(s == 1 || cost < min_cost) {
                            min_cost = cost;
                            Sy = s;
                            Vy = (SVy+s-1)/s;
                        }
                    }

                    delete [] Hs;
                    delete [] Hv;

                }
#ifdef OUTPUT_QMEASURE
                LOG(INFO) << "[TPR, HBU] (Sx, Sy, Vx, Vy) = " << Sx << " " << Sy << " " << Vx << " " << Vy << std::endl;
                LOG(INFO) << "[TPR, HBU] SVx, SVy, K = " << SVx << " " << SVy << " " << num_nodes << std::endl;
#endif

                // Sort the data 
                std::vector<int> idx;
                for(int i=0;i<num_points;++i)
                    idx.push_back(i);
                orderData(4, num_nodes, _mbrs, idx, 0, num_points, Sx, Sy, Vx, Vy);
                
#ifdef OUTPUT_QMEASURE
                LOG(INFO) << "[TPR, HBU] sorted" << std::endl;
#endif

                int st = 0, end;

                for(int i = 0; i < num_nodes; ++i) {
                    end = st + TPR_M; 
                    if(end > num_points) end = num_points;
                    // create a node that contains entries at [st...end) 
                    TPRTreeNode2D* node;
                    if(num_nodes == 1) {
                        node = this; // the only node at this level is the root
                    }
                    else {
                        node = new TPRTreeNode2D(hashMap, level/*TPR_H*/);
                    }
                    node->level = level;
                    for(int j=st;j<end;j++) {
                        //node->entries[j - st] = data[idx[j]];

                        node->mbrs[j-st] = _mbrs[idx[j]];
                        node->ptrs[j-st] = _ptrs[idx[j]];
                        

                        if(level == 0) { // child is a leaf
                            // update hashmap

#ifdef USE_NODE_ID
                            node->eid[j-st] = -1;
#endif
                            typename HashType::const_iterator iter
                                = hashMap->find(K::getOID(_ptrs[idx[j]].data));

                            if(iter == hashMap->end()) { //new entry
                                //hashMap[K::getOID(data[idx[j]].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(node, node->count);
                                (*hashMap)[K::getOID(_ptrs[idx[j]].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(node, node->count);
                            } 
                            else {                                
                                iter->second->parent = node;
                                iter->second->index = node->count;
                            }
                        }
                        else { // child is an internal node 
                            // update index/parent

#ifdef USE_NODE_ID                            
                            node->eid[j-st] = children[idx[j]]->id;
#endif

                            children[idx[j]]->setParent(node);// parent = node;
                            children[idx[j]]->setIndex(node->count); //index = node->count;
                        }
                        node->count++;
                    }

                    nodes.push_back(node);
                    st += TPR_M;
                }

                // prepare for the next level
                level++;
                children = nodes;
                nodes.clear();
            } while(num_nodes > 1);

        }
//#endif


        /** 
         * Returns a time-parameterized MBR which encloses 
         * all entry reactangles of this node at the time queryTick
         */
        TPRType getCover(int queryTick) {
            TPRType ans = mbrs[0]; //entries[0].mbr;
            ans.updatePositionToTick(queryTick);
            
            /*
            //LOG(DEBUG) << ans << std::endl;
            //LOG(DEBUG) << "after combining... " << entries[0].mbr << std::endl;
            */

            for (int i = 1; i < count; i++) {
                ans = combine2MBRs(ans, mbrs[i]/*entries[i].mbr*/, queryTick);
                /*
                //LOG(DEBUG) << ans << std::endl;
                //LOG(DEBUG) << "after combining... " << entries[i].mbr << std::endl;
                */
            }

            return ans;
        }
       
        // inline double min(double x, double y) const { return (x < y) ? x : y; }
        // inline double max(double x, double y) const { return (x < y) ? y : x; }

        // combine 2 MBRs that are NOT time-parameterized (just RType)
        //
        // TODO: Use method in Region2D. 
        RType combine2MBRs(const RType& r1, const RType& r2) {
            return RType(std::min(r1.getLow0(),  r2.getLow0()),
                         std::max(r1.getHigh0(), r2.getHigh0()),
                         std::min(r1.getLow1(),  r2.getLow1()),
                         std::max(r1.getHigh1(), r2.getHigh1()));
        }

        // combine 2 MBRs that are time-parameterized, at given queryTick
        TPRType combine2MBRs(const TPRType& r1, const TPRType& r2, int queryTick) { 
            RType future_pos1 = r1.getPosRTypeAt(queryTick);
            RType future_pos2 = r2.getPosRTypeAt(queryTick);

            RType future_pos = combine2MBRs(future_pos1, future_pos2);

            RType vel1 = r1.getVelRType();
            RType vel2 = r2.getVelRType();
            RType vel = combine2MBRs(vel1, vel2);
            
            TPRType ret = TPRType(&future_pos, &vel, queryTick);

            return ret;
        }

        /**
         * RTree Naive split algorithm pickEntry
         * pickEntry: 
         *    chooses an entry that will minimize the increased area
         *    area is time-dependent, so integrate it over time
         *    from [refTick] to [refTick + TPR_H] (tree parameter)
         */
        unsigned int pickEntry(const TPRType& mbr) {
            //LOG(DEBUG) << "[TPRNode, pickEntry]:" << *this << std::endl;

            int refTick = mbr.getRefTick();
            //TPRType currentMBR = entries[0].mbr;
            TPRType currentMBR = mbrs[0];

            double area = currentMBR.integratedArea(refTick, refTick + TPR_H);

            TPRType tempMBR = combine2MBRs(mbr, currentMBR, refTick);
            double minIncrease = tempMBR.integratedArea(refTick, refTick + TPR_H) - area;
            unsigned int ans = 0;

            for (int i = 1; i < count; i++) {
                //currentMBR = entries[i].mbr;
                currentMBR = mbrs[i];
                
                area = currentMBR.integratedArea(refTick, refTick + TPR_H);
                tempMBR = combine2MBRs(mbr, currentMBR, refTick);
                double increase = tempMBR.integratedArea(refTick, refTick + TPR_H) - area;
                if (minIncrease > increase) {
                    ans = i;
                    minIncrease = increase;
                }
            }
            
            //LOG(DEBUG) << "[TPRNode, pickEntry]: ans = " << ans << std::endl;
            return ans;
        }

        /* Algorithm Choose subtree: CL3*/
        TPRTreeNode2D* chooseSubTree(const TPRType& mbr) {
            //LOG(DEBUG) << "[TPRNode, chooseSubTree] node: " << *this << ", MBR: " << mbr << std::endl;
            unsigned int index = this->pickEntry(mbr);
            //LOG(DEBUG) << "[TPRNode, chooseSubTree] child Chosen: " << *(entries[index].childPtr) << std::endl;
            //return entries[index].childPtr;
            return ptrs[index].childPtr;

        }


        /*
         * This algorithm is associated with R-Tree split algorithm (splitNodeNaive)
         */
        
        void chooseSplitAxisNaive(int& index1, int& index2, int& queryTick, TPRType newMBR) {

            int indexLowX = TPR_M;
            int indexHighX = TPR_M;

            double highestLowX = newMBR.getLow0At(queryTick);
            double lowestHighX = newMBR.getHigh0At(queryTick);

            double min = highestLowX;
            double max = lowestHighX;

            // Find the farthest pair in dimension X
            for (int i = 0; i < TPR_M; i++) {
                // int lowX = entries[i].mbr.getLow0At(queryTick);
                // int highX = entries[i].mbr.getHigh0At(queryTick);
                int lowX = mbrs[i].getLow0At(queryTick);
                int highX = mbrs[i].getHigh0At(queryTick);

                if (highestLowX < lowX) {
                    highestLowX = lowX;;
                    indexLowX = i;
                }
                if (lowestHighX > highX) {
                    lowestHighX = highX;
                    indexHighX = i;
                }

                if (min > lowX) {
                    min  = lowX;
                }

                if (max < highX) {
                    max = highX;
                }
            }

            // Normalize the separation in dimension X
            double widX = (highestLowX - lowestHighX) / (max - min);

            //LOG(DEBUG) << "[TPR, chooseSplitAxisNaive]: dimension X: "                     << "indexLowX = " << indexLowX << " : "                       << "indexHighX = " << indexHighX << " : "                       << "widX = " << widX                       << std::endl;


            int indexLowY = TPR_M;
            int indexHighY = TPR_M;
            // double highestLowY = newEntry.mbr.getLow1At(queryTick);
            // double lowestHighY = newEntry.mbr.getHigh1At(queryTick);

            double highestLowY = newMBR.getLow1At(queryTick);
            double lowestHighY = newMBR.getHigh1At(queryTick);


            min = highestLowY;
            max = lowestHighY;
            // Find the farthest pair in dimension Y
            for (int i = 0; i < TPR_M; i++) {

                // int lowY = entries[i].mbr.getLow1At(queryTick);
                // int highY = entries[i].mbr.getHigh1At(queryTick);
                int lowY = mbrs[i].getLow1At(queryTick);
                int highY = mbrs[i].getHigh1At(queryTick);

                if (highestLowY < lowY) {
                    highestLowY = lowY;;
                    indexLowY = i;
                }
                if (lowestHighY > highY) {
                    lowestHighY = highY;
                    indexHighY = i;
                }

                if (min > lowY) {
                    min  = lowY;
                }

                if (max < highY) {
                    max = highY;
                }
            }

            // Normalize the separation in dimension Y
            double widY = (highestLowY - lowestHighY) / (max - min);

            //LOG(DEBUG) << "[TPR, chooseSplitAxisNaive]: dimension Y: "                       << "indexLowY = " << indexLowY << " : "                       << "indexHighY = " << indexHighY << " : "                       << "widY = " << widY                       << std::endl;

            if (widX > widY) {
                index1 = indexLowX;
                index2 = indexHighX;
            } else {
                index1 = indexLowY;
                index2 = indexHighY;
            }
        }
        

        /**
         * R-Tree split algorithm (naive)
         */
         
        void splitNodeNaive(int queryTick, 
                            //const TPRTreeEntry2D& newEntry, 
                            TPRType newMBR,
                            Ptr newPTR,
                            TPRTreeNode2D** splitNode1, 
                            TPRTreeNode2D** splitNode2) {
            //LOG(DEBUG) << "[TPR, splitNode] Node: " << *this << std::endl;
            //LOG(DEBUG) << "\t newEntry: " << newEntry.mbr << std::endl;
           
            int seed1, seed2;
            //chooseSplitAxisNaive(seed1, seed2, queryTick, newEntry);
            chooseSplitAxisNaive(seed1, seed2, queryTick, newMBR);
            // if seed1 or seed2 is equal to TPR_MAX_ENTREES,
            // newEntry should be used

            //LOG(DEBUG) << "[TPR, splitNode] seed1 = "                        << seed1 << " , " << "seed2 = "                        << seed2 << std::endl;

            *splitNode1 = new TPRTreeNode2D(hashMap);
            *splitNode2 = new TPRTreeNode2D(hashMap);
            
            unsigned int count1 = 0;
            unsigned int count2 = 0;

            // put the first half of the entries to splitNode1
            int ind = 0;
            int i = 0;
            for ( ; i < (TPR_M+1)/2; i++) {
                if (seed2 != i) {

                    //(*splitNode1)->entries[ind] = entries[i];

                    (*splitNode1)->ptrs[ind] = ptrs[i];
                    (*splitNode1)->mbrs[ind] = mbrs[i];

                    if (isInternalNode()) {
                        // (((*splitNode1)->entries[ind]).childPtr)->index = ind;
                        // (((*splitNode1)->entries[ind]).childPtr)->parent = *splitNode1;
                        (((*splitNode1)->ptrs[ind]).childPtr)->setIndex(ind);// = ind;
                        (((*splitNode1)->ptrs[ind]).childPtr)->setParent(*splitNode1);// = *splitNode1;
                    }
#ifdef DELETE_BY_HASH
                    else {
                        // typename HashType::const_iterator iter 
                        //     = hashMap.find(K::getOID((*splitNode1)->entries[ind].data));

                        typename HashType::const_iterator iter 
                            = hashMap->find(K::getOID((*splitNode1)->ptrs[ind].data));

                        if(iter == hashMap->end()) { //new entry
                            // hashMap[K::getOID((*splitNode1)->entries[ind].data)]
                            //     = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode1, ind);

                            (*hashMap)[K::getOID((*splitNode1)->ptrs[ind].data)]
                                = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode1, ind);

                        } 
                        else {

                            iter->second->parent = *splitNode1;
                            iter->second->index = ind;
                        }
                    }
#endif

                    ind++;
                    count1++;
                }
            }

            if (seed1 >= (TPR_M + 1)/2) { // if seed1 has not been added

                if(seed1 == TPR_M) {
                    (*splitNode1)->mbrs[ind] = newMBR;
                    (*splitNode1)->ptrs[ind] = newPTR;
                }
                else {
                    (*splitNode1)->mbrs[ind] = mbrs[seed1];
                    (*splitNode1)->ptrs[ind] = ptrs[seed1];
                }

                //(*splitNode1)->entries[ind] = (seed1 == TPR_M ? newEntry : entries[seed1]);
                
                if (isInternalNode()) {
                    // (((*splitNode1)->entries[ind]).childPtr)->index = ind;
                    // (((*splitNode1)->entries[ind]).childPtr)->parent = *splitNode1;

                    (((*splitNode1)->ptrs[ind]).childPtr)->setIndex(ind);
                    (((*splitNode1)->ptrs[ind]).childPtr)->setParent(*splitNode1);

                }
#ifdef DELETE_BY_HASH
                else {
                    // typename HashType::const_iterator iter
                    //     = hashMap.find(K::getOID((*splitNode1)->entries[ind].data));

                    typename HashType::const_iterator iter
                        = hashMap->find(K::getOID((*splitNode1)->ptrs[ind].data));

                    if(iter == hashMap->end()) { //new entry
                        //hashMap[K::getOID((*splitNode1)->entries[ind].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode1, ind);
                        (*hashMap)[K::getOID((*splitNode1)->ptrs[ind].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode1, ind);
                    } 
                    else {
                        iter->second->parent = *splitNode1;
                        iter->second->index = ind;
                    }
                }
#endif

                count1++;
            }


            //LOG(DEBUG) << "[TPR, splitnode] spnode1 mbr: " << (*splitNode1)->entries[0].mbr << std::endl;
            //(*splitNode1)->count = count1;
            (*splitNode1)->count = count1;

            //(*splitNode1)->level = this->level;
            (*splitNode1)->setLevel(this->level);

            // put the second half of the entries to splitNode2
            ind = 0; // reset ind
            for( ; i < TPR_M + 1; i++) {
                if (seed1 != i) {
                    //(*splitNode2)->entries[ind] = (i==TPR_M ? newEntry : entries[i]);

                    if(i == TPR_M) {
                        (*splitNode2)->mbrs[ind] = newMBR;
                        (*splitNode2)->ptrs[ind] = newPTR;
                    }
                    else {
                        (*splitNode2)->mbrs[ind] = mbrs[i];
                        (*splitNode2)->ptrs[ind] = ptrs[i];
                    }

                    //(*splitNode2)->entries[ind] = (i==TPR_M ? newEntry : entries[i]);

                    if (isInternalNode()) {
                        // (((*splitNode2)->entries[ind]).childPtr)->index = ind;
                        // (((*splitNode2)->entries[ind]).childPtr)->parent = *splitNode2;

                        (((*splitNode2)->ptrs[ind]).childPtr)->setIndex(ind);
                        (((*splitNode2)->ptrs[ind]).childPtr)->setParent(*splitNode2);
                    }
#ifdef DELETE_BY_HASH
                    else {
                        // typename HashType::const_iterator iter
                        //     = hashMap.find(K::getOID((*splitNode2)->entries[ind].data));

                        typename HashType::const_iterator iter
                            = hashMap->find(K::getOID((*splitNode2)->ptrs[ind].data));

                        if(iter == hashMap->end()) { //new entry
                            //hashMap[K::getOID((*splitNode2)->entries[ind].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode2, ind);
                            (*hashMap)[K::getOID((*splitNode2)->ptrs[ind].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode2, ind);
                        } 
                        else {
                            iter->second->parent = *splitNode2;
                            iter->second->index = ind;
                        }
                    }
#endif
                    ind++;
                    count2++;
                }
            }

            if (seed2 < TPR_M/2){
                //(*splitNode2)->entries[ind] = (seed2==TPR_M ? newEntry : entries[seed2]);

                if(seed2 == TPR_M) {
                    (*splitNode2)->mbrs[ind] = newMBR;
                    (*splitNode2)->ptrs[ind] = newPTR;
                }
                else {
                    (*splitNode2)->mbrs[ind] = mbrs[seed2];
                    (*splitNode2)->ptrs[ind] = ptrs[seed2];
                }


                if (isInternalNode()) {
                    // (((*splitNode2)->entries[ind]).childPtr)->index = ind;
                    // (((*splitNode2)->entries[ind]).childPtr)->parent = *splitNode2;

                    (((*splitNode2)->ptrs[ind]).childPtr)->setIndex(ind);
                    (((*splitNode2)->ptrs[ind]).childPtr)->setParent(*splitNode2);
                }
#ifdef DELETE_BY_HASH
                else {
                    // typename HashType::const_iterator iter
                    //     = hashMap.find(K::getOID((*splitNode2)->entries[ind].data));

                    typename HashType::const_iterator iter
                        = hashMap->find(K::getOID((*splitNode2)->ptrs[ind].data));
                    

                    if(iter == hashMap->end()) { //new entry
                        //hashMap[K::getOID((*splitNode2)->entries[ind].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode2, ind);
                        (*hashMap)[K::getOID((*splitNode2)->ptrs[ind].data)] = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode2, ind);
                    } 
                    else {
                        iter->second->parent = *splitNode2;
                        iter->second->index = ind;
                    }
                }
#endif

                count2++;
            }

            //LOG(DEBUG) << "[TPR, splitnode] spnode2 mbr: " << (*splitNode2)->entries[0].mbr << std::endl;
            (*splitNode2)->count = count2;
            (*splitNode2)->level = this->level;

            //LOG(DEBUG) << "[TPR, splitnode] spnode1: " << *splitNode1 << std::endl;     
            //LOG(DEBUG) << "[TPR, splitnode] spnode2: " << *splitNode2 << std::endl;
            //LOG(DEBUG) << "[TPR, splitNode] the node " << *this << "  has been split into: " << std::endl                       << "\t" << *splitNode1 << " and " << *splitNode2 << std::endl;

        }
        

        /**
         * this class is used to sort points by given coordinate
         * - used in R* tree split algorithm
         * - it stores in what coordinate the points should be sorted
         */
        class mbrComp {
            private:
            const std::vector<TPRType*> &_mbrs;
            const int _sort_axis;

            public:

            mbrComp(const std::vector<TPRType*> & mbrs, int sort_axis) : _mbrs(mbrs), _sort_axis(sort_axis) {}

            /* For given axis, sort the entries
             * by the lower then by the upper value
             */
            inline bool operator()(int a, int b) const {
                if(_sort_axis == 0) {// sort by x       
                    if(_mbrs.at(a)->getLow0P() == _mbrs.at(b)->getLow0P())
                        return _mbrs.at(a)->getHigh0P() < _mbrs.at(b)->getHigh0P();
                    else 
                        return _mbrs.at(a)->getLow0P() < _mbrs.at(b)->getLow0P();
                }
                else if(_sort_axis == 1) {//sort by y
                    if(_mbrs.at(a)->getLow1P() == _mbrs.at(b)->getLow1P())
                        return _mbrs.at(a)->getHigh1P() < _mbrs.at(b)->getHigh1P();
                    else 
                        return _mbrs.at(a)->getLow1P() < _mbrs.at(b)->getLow1P();
                }
                else if(_sort_axis == 2) {//sort by vx
                    if(_mbrs.at(a)->getLow0V() == _mbrs.at(b)->getLow0V())
                        return _mbrs.at(a)->getHigh0V() < _mbrs.at(b)->getHigh0V();
                    else 
                        return _mbrs.at(a)->getLow0V() < _mbrs.at(b)->getLow0V();
                }
                else {//sort by vy
                    if(_mbrs.at(a)->getLow1V() == _mbrs.at(b)->getLow1V())
                        return _mbrs.at(a)->getHigh1V() < _mbrs.at(b)->getHigh1V();
                    else 
                        return _mbrs.at(a)->getLow1V() < _mbrs.at(b)->getLow1V();
                }
            }
        };

        /*
         * instead of just sorting d-dimensional (position) coordinates,
         * also consider d-dimensional velocity coordinates (so total 2*d dimensions)
         *
         * load-time bounding rectangles use positions at t_load
         * and update-time bounding rectangles use positions at t_upd 
         */
        void splitNodeRStar(int queryTick, 
                            //const TPRTreeEntry2D& newEntry, 
                            TPRType newMBR,
                            Ptr newPTR,
                            TPRTreeNode2D** splitNode1, 
                            TPRTreeNode2D** splitNode2) {

            //TPRType newEntryMBR = newEntry.mbr;
            std::vector<TPRType*> _mbrs;

            for(int i = 0 ; i < TPR_M ; ++i) {
                //mbrs.push_back( &(this->entries[i].mbr) );
                _mbrs.push_back( &(this->mbrs[i]) );
            }
            _mbrs.push_back( &newMBR );

            std::vector<int> mbrIndex;
            for(int i = 0 ; i <= TPR_M ; ++i) {
                mbrIndex.push_back(i);
            }
            
            int splitAxis = -1;
            double min_margin_sum = 1.e100;
            /* S1: Invoke ChooseSplitAxis to determine the axis */
            for(int axis = 0 ; axis < 4; ++axis) {    
                /* CSA1: for each axis, sort */
                // {0, 1, 2, 3} {x, y, vx, vy}
                std::sort(mbrIndex.begin(), mbrIndex.end(), mbrComp(_mbrs, axis));

                /* CSA2: Compute the sum of margine values (S) of all distributions */
                double margin_sum = 0.0;

                // group 1's margin sums are computed in linear time
                {
                    TPRType mbr_g1 = *(_mbrs.at(mbrIndex[0]));
                    mbr_g1.updatePositionToTick(queryTick);
                    for(int i = 1 ; i < TPR_m ; ++i) {
                        mbr_g1 = combine2MBRs(mbr_g1, *(_mbrs.at(mbrIndex[i])), queryTick);
                    }

                    for(int k = 1 ; k <= (TPR_M - 2*TPR_m + 2); ++k) {
                        // k-th distribution: (TPR_m + (k-1), rest)
                        margin_sum += mbr_g1.margin();
                        mbr_g1 = combine2MBRs(mbr_g1, *(_mbrs.at(mbrIndex[TPR_m + k - 1])), queryTick);
                    }
                }

                // group 2's margin sums are computed in linear time
                {
                    TPRType mbr_g2 = *(_mbrs.at(mbrIndex[TPR_M]));
                    mbr_g2.updatePositionToTick(queryTick);
                    // largest k = TPR_M - 2*TPR_m + 2
                    // TPR_m+(k-1) = TPR_M - TPR_m + 1
                    for(int i = TPR_M - TPR_m + 1 ; i < TPR_M; ++i) {
                        mbr_g2 = combine2MBRs(mbr_g2, *(_mbrs.at(mbrIndex[i])), queryTick);
                    }
                    for(int k = TPR_M - TPR_m  ; k >= TPR_m ; --k) {
                        // k-th distribution: (TPR_m + (k-1), rest)
                        margin_sum += mbr_g2.margin();
                        mbr_g2 = combine2MBRs(mbr_g2, *(_mbrs.at(mbrIndex[k])), queryTick);
                    }

                }

                if(splitAxis == -1 || min_margin_sum > margin_sum) {
                    min_margin_sum = margin_sum;
                    splitAxis = axis;
                }

                /* This code runs in quadratic time
                for(int k = 1 ; k <= (TPR_M - 2*TPR_m + 2); ++k) {
                    // k-th distribution: (TPR_m + (k-1), rest)
                    TPRType mbr_g1, mbr_g2;

                    mbr_g1 = *(mbrs.at(mbrIndex[0]));
                    mbr_g1.updatePositionToTick(queryTick);
                    for(int i = 1 ; i < TPR_m + (k-1) ; ++i) {
                        mbr_g1 = combine2MBRs(mbr_g1, *(mbrs.at(mbrIndex[i])), queryTick);
                    }

                    mbr_g2 = *(mbrs.at(mbrIndex[TPR_m+(k-1)]));
                    mbr_g2.updatePositionToTick(queryTick);

                    for(int i = TPR_m+k ; i <= TPR_M ; ++i) {
                        mbr_g2 = combine2MBRs(mbr_g2, *(mbrs.at(mbrIndex[i])), queryTick);
                    }
                    
                    margin_sum += mbr_g1.margin();
                    margin_sum += mbr_g2.margin();
                }
                if(splitAxis == -1 || min_margin_sum > margin_sum) {
                    min_margin_sum = margin_sum;
                    splitAxis = axis;
                }
                */

            }

            //LOG(DEBUG) << "Split Axis = " << splitAxis << ", with S = " << min_margin_sum << std::endl;

            /* S2: Invoke ChooseSplitIndex to determine the distribution */
            /* CSI1: choose the distribution (k) with the minimum overlap-value 
             *       resolve ties by choosing the one with minimum area-value
             */

            std::sort(mbrIndex.begin(), mbrIndex.end(), mbrComp(_mbrs, splitAxis));

            int splitIndex = -1;
            double min_area = 1.e100, min_area_sum;
            for(int k = 1; k <= (TPR_M - 2*TPR_m + 2) ; k++) {
                TPRType mbr_g1, mbr_g2;

                mbr_g1 = *(_mbrs.at(mbrIndex[0]));
                mbr_g1.updatePositionToTick(queryTick);
                for(int i = 1 ; i < TPR_m + (k-1) ; ++i) {
                    mbr_g1 = combine2MBRs(mbr_g1, *(_mbrs.at(mbrIndex[i])), queryTick);
                }

                mbr_g2 = *(_mbrs.at(mbrIndex[TPR_m+(k-1)]));
                mbr_g2.updatePositionToTick(queryTick);

                for(int i = TPR_m+k ; i <= TPR_M ; ++i) {
                    mbr_g2 = combine2MBRs(mbr_g2, *(_mbrs.at(mbrIndex[i])), queryTick);
                }

                RType r1 = mbr_g1.getPosRType();
                RType r2 = mbr_g2.getPosRType();
                RType overlap = r1.getIntersection(&r2);
                double area = overlap.area();
                double area_sum = r1.area() + r2.area();
                if((splitIndex == -1) || (min_area > area) || (min_area == area && min_area_sum > area_sum)) {
                    splitIndex = k;
                    min_area = area;
                    min_area_sum = area_sum;
                }
            }

            //LOG(DEBUG) << "Split Index = " << splitIndex << ", with area = " << min_area << ", with area_sum = " << min_area_sum << std::endl;

            /* S3: Distribute the entries */

            *splitNode1 = new TPRTreeNode2D(hashMap);
            *splitNode2 = new TPRTreeNode2D(hashMap);

            //std::sort(mbrIndex.begin(), mbrIndex.end(), mbrComp(mbrs, splitAxis)); //already sorted
            //use k = splitIndex
            {
                int ind1 = 0, count1 = 0;;
                for(int i = 0 ; i < TPR_m + (splitIndex-1) ; ++i) {
                    if(mbrIndex[i] == TPR_M) {
                        //(*splitNode1)->entries[ind1] = newEntry;
                        (*splitNode1)->mbrs[ind1] = newMBR;
                        (*splitNode1)->ptrs[ind1] = newPTR;
                    }
                    else {
                        //(*splitNode1)->entries[ind1] = entries[mbrIndex[i]];
                        (*splitNode1)->mbrs[ind1] = mbrs[mbrIndex[i]];
                        (*splitNode1)->ptrs[ind1] = ptrs[mbrIndex[i]];
                    }
                    if (isInternalNode()) {
                        // (((*splitNode1)->entries[ind1]).childPtr)->index = ind1;
                        // (((*splitNode1)->entries[ind1]).childPtr)->parent = *splitNode1;

                        (((*splitNode1)->ptrs[ind1]).childPtr)->setIndex(ind1);
                        (((*splitNode1)->ptrs[ind1]).childPtr)->setParent(*splitNode1);

#ifdef USE_NODE_ID
                        ((*splitNode1)->eid[ind1]) = (*splitNode1)->ptrs[ind1].childPtr->id;
#endif                        
                    }
#ifdef DELETE_BY_HASH
                    else {
                        // typename HashType::const_iterator iter 
                        //     = hashMap.find(K::getOID((*splitNode1)->entries[ind1].data));

                        typename HashType::const_iterator iter 
                            = hashMap->find(K::getOID((*splitNode1)->ptrs[ind1].data));

                        if(iter == hashMap->end()) { //new entry
                            // hashMap[K::getOID((*splitNode1)->entries[ind1].data)] 
                            //     = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode1, ind1);
                            (*hashMap)[K::getOID((*splitNode1)->ptrs[ind1].data)]
                                = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode1, ind1);
                        } 
                        else {
                            iter->second->parent = *splitNode1;
                            iter->second->index = ind1;
                        }
#ifdef USE_NODE_ID
                        ((*splitNode1)->eid[ind1]) = -1;//(*splitNode1)->ptrs[ind1].childPtr->id;
#endif
                    }
#endif
                    ind1++;
                    count1++;
                }
                (*splitNode1)->count = count1;
                (*splitNode1)->level = this->level;//level = this->level;

            }

            {
                int ind2 = 0, count2 = 0;
                for(int i = TPR_m+splitIndex-1 ; i <= TPR_M ; ++i) {
                    if(mbrIndex[i] == TPR_M) {
                        //(*splitNode2)->entries[ind2] = newEntry;

                        (*splitNode2)->mbrs[ind2] = newMBR;
                        (*splitNode2)->ptrs[ind2] = newPTR;
                    }
                    else {
                        //(*splitNode2)->entries[ind2] = entries[mbrIndex[i]];

                        (*splitNode2)->mbrs[ind2] = mbrs[mbrIndex[i]];
                        (*splitNode2)->ptrs[ind2] = ptrs[mbrIndex[i]];
                    }
                    if (isInternalNode()) {
                        // (((*splitNode2)->entries[ind2]).childPtr)->index = ind2;
                        // (((*splitNode2)->entries[ind2]).childPtr)->parent = *splitNode2;

                        (((*splitNode2)->ptrs[ind2]).childPtr)->setIndex(ind2);
                        (((*splitNode2)->ptrs[ind2]).childPtr)->setParent(*splitNode2);

#ifdef USE_NODE_ID

                        ((*splitNode2)->eid[ind2]) = (*splitNode2)->ptrs[ind2].childPtr->id;
#endif
                    }
#ifdef DELETE_BY_HASH
                    else {
                        
#ifdef USE_NODE_ID
                        ((*splitNode2)->eid[ind2]) = -1;//(*splitNode2)->ptrs[ind2].childPtr->id;
#endif
                        // typename HashType::const_iterator iter
                        //     = hashMap.find(K::getOID((*splitNode2)->entries[ind2].data));

                        typename HashType::const_iterator iter
                            = hashMap->find(K::getOID((*splitNode2)->ptrs[ind2].data));


                        if(iter == hashMap->end()) { //new entry
                            // hashMap[K::getOID((*splitNode2)->entries[ind2].data)] 
                            //     = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode2, ind2);

                            (*hashMap)[K::getOID((*splitNode2)->ptrs[ind2].data)] 
                                = new NodeIndex<U,K,TPR_m,TPR_M>(*splitNode2, ind2);
                        } 
                        else {
                            iter->second->parent = *splitNode2;
                            iter->second->index = ind2;

                            //iter->second->setParent(*splitNode2);
                            //iter->second->setIndex(ind2);
                        }
                    }
#endif
                    ind2++;
                    count2++;
                }
                // (*splitNode2)->count = count2;
                // (*splitNode2)->level = this->level;

                (*splitNode2)->count = count2;
                (*splitNode2)->setLevel(this->level);
            }

            DBUTL_ASSERT(splitNode1 != NULL);
            DBUTL_ASSERT(splitNode2 != NULL);

            //LOG(DEBUG) << *splitNode1 << std::endl;
            //LOG(DEBUG) << *splitNode2 << std::endl;

        } // splitNodeRStar

        /** 
         * Algorithm AdjustTree: 
         *  - adjust parent pointers, parent indices
         *  - upon split, properly propagate the adjustment upward the tree
         */
        bool adjustTree(int queryTick, 
                        TPRTreeNode2D<U, K, TPR_m, TPR_M>** splitNode1, 
                        TPRTreeNode2D<U, K, TPR_m, TPR_M>** splitNode2) {

            //LOG(DEBUG) << "[TPR, adjustTree (node)]:" << *this << std::endl;
            if (this->isRoot()) {
                if ((*splitNode1) != NULL) {
                //if (this->isSplit()) { 
                    // the root was split

                    this->count = 0;
                    this->setLevel(this->getLevel() + 1);
                    this->setNewID();

                    TPRTreeNode2D<U, K, TPR_m, TPR_M>* child1 = *splitNode1;
                    TPRTreeNode2D<U, K, TPR_m, TPR_M>* child2 = *splitNode2;
                    //LOG(DEBUG) << " (chilld1): " << *child1 << std::endl;
                    //LOG(DEBUG) << " (chilld2): " << *child2 << std::endl;

                    this->addChild(child1, queryTick, this);
                    this->addChild(child2, queryTick, this);

                    if(child1->isInternalNode()) 
                        adjustParentPointers(child1);

                    if(child2->isInternalNode())
                        adjustParentPointers(child2);
                    

                    //LOG(DEBUG) << "this(root) = " << *this << std::endl;

                    //LOG(DEBUG) << " (chilld1): " << *(this->entries[0].childPtr) << ", parent = " << *(this->entries[0].childPtr->parent) << std::endl;
                    //LOG(DEBUG) << " (chilld2): " << *(this->entries[1].childPtr) << ", parent = " << *(this->entries[1].childPtr->parent) << std::endl;

                    //this->resetSplitNodes();
                    *splitNode1 = NULL;
                    *splitNode2 = NULL;

                    //LOG(DEBUG) << "[TPR, adjustTree (node)]: new Root: " << *this << std::endl;
                }
                //LOG(DEBUG) << "[TPR, adjustTree (node)] - returning..." << std::endl;              

                return false;
            }

            DBUTL_ASSERT(parent != NULL);
            if ((*splitNode1) != NULL) {
            //if (this->isSplit()) {

                TPRTreeNode2D<U, K, TPR_m, TPR_M>* childNode1 = *splitNode1;
                TPRTreeNode2D<U, K, TPR_m, TPR_M>* childNode2 = *splitNode2;

                //LOG(DEBUG) << "[TPR, adjustTree (node)] this: " << *this << ", this->parent: " << *(this->parent) << std::endl;

                //childNode1->parent = this->parent;
                childNode1->setParent(this->parent);

                //LOG(DEBUG) << "[parent = " << *(this->parent) << "], [this = " << *this << "]" << std::endl;
                //childNode2->parent = this->parent;
                childNode2->setParent(this->parent);
                //LOG(DEBUG) << "[parent = " << *(this->parent) << "], [this = " << *this << "]" << std::endl;

                //this->resetSplitNodes();
                *splitNode1 = NULL;
                *splitNode2 = NULL;

                /* AT3: adjust co//vering rectangle in parent entry */
                //LOG(DEBUG) << "[TPR, adjustTree (node)] - update co//vering MBR" << std::endl;

                TPRType childNode1MBR = childNode1->getCover(queryTick);

                //LOG(DEBUG) << *childNode1 << " " << childNode1MBR << std::endl;

                TPRType childNode2MBR = childNode2->getCover(queryTick);

                //LOG(DEBUG) << *childNode2 << " " << childNode2MBR << std::endl;

                /* AT4: propagate node split upward. */
                //LOG(DEBUG) << "[TPR, adjustTree (node)] performing AT4 at parent = " << parent << " " << *parent << std::endl;


                parent->overwriteEntry(childNode1MBR, childNode1, this->index);

                //childNode2->index = (parent->count); 
                childNode2->setIndex(parent->count);

                //std::cout << "Before adjustTree Call" << std::endl;
                parent->addEntry(childNode2MBR, childNode2, splitNode1, splitNode2);




                //std::cout << "After adjustTree Call" << std::endl;                
            
                //LOG(DEBUG) << "[TPR, adjustTree (node)] parent = " << *parent << ", child1 parent = " << *(childNode1->parent) << std::endl;
                //LOG(DEBUG) << "[TPR, adjustTree (node)] parent = " << *parent << ", child2 parent = " << *(childNode2->parent) << std::endl;

                //LOG(DEBUG) << "[TPR, adjustTree (node)] - returning true..." << std::endl;

                return true;
            } else {
                /* AT3: adjust covering rectangle in parent entry */

                TPRType newMBR = this->getCover(queryTick);

                //(parent->entries[this->index]).mbr = newMBR;
                parent->mbrs[this->index] = newMBR;

                return false;
            }


        }

        void adjustParentPointers(TPRTreeNode2D<U, K, TPR_m, TPR_M>* node) {
            //LOG(DEBUG) << "[adjustParentPointers] adjusting pointers for nodes under: " << *node << std::endl;
            for(int i = 0 ; i < node->count ; ++i) {
                //node->entries[i].childPtr->parent = node;
                node->ptrs[i].childPtr->parent = node;
            }
        }

        /**
         * returns true if split was performed
         */
        bool chooseLeafToInsert(const TPRType& mbr, 
                                U* const ptrData, 
                                TPRTreeNode2D** splitNode1, 
                                TPRTreeNode2D** splitNode2) {




            ///*LOG(DEBUG)*/std::cout << "[TPRNode, chooseLeafToInsert()]:" << *this;
            if (this->isLeaf()) {
                ///*LOG(DEBUG)*/std::cout << " (leaf node)" << std::endl;

                addEntry(mbr, ptrData, splitNode1, splitNode2);

                return adjustTree(mbr.getRefTick(), splitNode1, splitNode2);
            } else { 
                ///*LOG(DEBUG)*/std::cout << " (internal node)" << std::endl;
                TPRTreeNode2D<U, K, TPR_m, TPR_M>* childNode = this->chooseSubTree(mbr);
                
                //std::cout << "recursing" << std::endl;
                if(childNode->chooseLeafToInsert(mbr, ptrData, splitNode1, splitNode2)) {
                    delete childNode;
                }


                return adjustTree(mbr.getRefTick(), splitNode1, splitNode2);
            }
        }



#ifdef DELETE_BY_HASH
        // Deletion propagates IF the child gets underfull as a result of deletion
        bool postDelete(TPRTreeNode2D<U, K, TPR_m, TPR_M>* child) {

            if(child->count < TPR_m) {

                //delete 'child' node
                this->count--;
                if(child->index < this->count) {

                    //this->entries[child->index] = entries[this->count];
                    this->mbrs[child->index] = mbrs[this->count];
                    this->ptrs[child->index] = ptrs[this->count];


                    //this->entries[child->index].childPtr->index = child->index;
                    this->ptrs[child->getIndex()].childPtr->setIndex(child->getIndex());


                }
                delete child;

                if(this->parent != NULL) { 

                    // propagate upward
                    return this->parent->postDelete(this);
                }
            }

            return true;
        }

        bool deleteDataByOID(typename K::IDType oid, int indexData) {

            DBUTL_ASSERT(this->isLeaf());
            DBUTL_ASSERT(this->count > indexData);
            DBUTL_ASSERT(K::getOID(this->ptrs[indexData].data) == oid);

            // Delete the data in this leaf node
            this->count--;

            if(indexData < this->count) {

                //entries[indexData] = entries[this->count];

                mbrs[indexData] = mbrs[this->count];
                ptrs[indexData] = ptrs[this->count];

                //hashMap[K::getOID(entries[indexData].data)]->index = indexData;
                (*hashMap)[K::getOID(ptrs[indexData].data)]->index = indexData;
            }

            //TODO: one optimization: if we know that the set of OIDs is not
            //changing over time, we do NOT need to delete entries in hashMap

            delete (hashMap->find(oid))->second;
            hashMap->erase(hashMap->find(oid));

            if(this->parent == NULL) {
                // 'this' is a leaf, root node 
                return true;
            }

            return this->parent->postDelete(this);
        }
        
        bool deleteDataByOID(typename K::IDType oid) {

            typename HashType::const_iterator iter = hashMap->find(oid);
            if(iter == hashMap->end()) {
                // not found
                return false;
            }
            NodeIndex<U,K,TPR_m,TPR_M>* nodeIndex = (*hashMap)[oid];
            DBUTL_ASSERT(nodeIndex != NULL);

            return nodeIndex->parent->deleteDataByOID(oid, nodeIndex->index);
        }

#else
        // this is a deletion algorithm by traversing the tree, without using the hash look-up
        bool searchToDelete(int queryTick, RType* const r, U* const point) {

            if (isLeaf()) {

                // S2: Search leaf node.
                for (int i = 0; i < count; i++) {                    
                    //if (entries[i].mbr.satisfiesQuery(r, queryTick)) {
                    if (mbrs[i].satisfiesQuery(r, queryTick)) {
                        if(K::getOID(ptrs[i].data) != K::getOID(point)) {
                            continue;
                        }

                        count --;
                        if(i<count) {
                            //entries[i] = entries[count];
                            mbrs[i] = mbrs[count];
                            ptrs[i] = ptrs[count];
                        }
                        return true;
                    }
                }

                return false;
            } else {

                // S1: Search subtrees recursively.
                for (int i = 0; i < count; i++) {

                    //if (entries[i].mbr.satisfiesQuery(r, queryTick)) {
                    if (mbrs[i].satisfiesQuery(r, queryTick)) {

                        //if((entries[i].childPtr)->searchToDelete( queryTick, r, point)) {
                        if((ptrs[i].childPtr)->searchToDelete( queryTick, r, point)) {
                            //if((entries[i].childPtr)->count < TPR_m) {
                            if((ptrs[i].childPtr)->count < TPR_m) {
                                //delete entries[i].childPtr;
                                delete ptrs[i].childPtr;
                                count--;
                                if(i<count) {
                                    //entries[i] = entries[count];

                                    mbrs[i] = mbrs[count];
                                    ptrs[i] = ptrs[count];

                                    //(entries[i].childPtr)->index = i;
                                    (ptrs[i].childPtr)->setIndex(i);
                                }
                            }
                            else {
                                //entries[i].mbr = (entries[i].childPtr)->getCover(queryTick);
                                mbrs[i] = (ptrs[i].childPtr)->getCover(queryTick);
                            }
                            return true;
                        }
                    }
                }
                return false;
            }
        }
#endif

        /**
         * Add a child to a node, update the index accordingly.
         * Must be called only if the node has room for a new entry
         */
        void addChild(TPRTreeNode2D* childPtr, int queryTick, TPRTreeNode2D* parentPtr) {
            DBUTL_ASSERT(count < TPR_M);

            //TPRTreeEntry2D newEntry(childPtr->getCover(queryTick), childPtr);
            //entries[count] = newEntry;
            mbrs[count] = childPtr->getCover(queryTick);
            ptrs[count].childPtr = childPtr;
            
#ifdef USE_NODE_ID
            eid[count] = childPtr->id;
#endif

            childPtr->setIndex(count);
            childPtr->setParent(parentPtr);
            count++;
        }

        /* Add record to a node: I2 */
        void addEntry(//const TPRTreeEntry2D& newEntry, 
                      TPRType newMBR,
                      Ptr newPTR,
                      TPRTreeNode2D** splitNode1, 
                      TPRTreeNode2D** splitNode2,
                      bool internal
#ifdef USE_NODE_ID
                      , int _eid
#endif
) {



            if (count < TPR_M) {  // have room to insert new entry
                //entries[count] = newEntry;

                mbrs[count] = newMBR;
                ptrs[count] = newPTR;
                
#ifdef USE_NODE_ID
                eid[count] = _eid;
#endif

                //TODO: one optimization: if we know that the set of OIDs is
                //not changing over time, we do NOT need to delete entries in
                //hashMap
#ifdef DELETE_BY_HASH
                //if(newPTR.data != NULL) {
                if(!internal) {

                    //DBUTL_ASSERT(hashMap.find(K::getOID(newEntry.data)) == hashMap.end());

                    if(hashMap->find(K::getOID(newPTR.data)) != hashMap->end()) {
                        std::cout << std::endl << "hashMap..." << std::endl;
                        typename HashType::iterator it; 

                        for(it = hashMap->begin(); it != hashMap->end(); it++) {
                            std::cout << it->first << ": " << *(it->second)->parent 
                                      << " " << (it->second)->index << std::endl;
                        }


                        std::cout << std::endl << "point: " << *newPTR.data << std::endl;
                    }

                    DBUTL_ASSERT(hashMap->find(K::getOID(newPTR.data)) == hashMap->end());
                    //hashMap[K::getOID(newEntry.data)] = new NodeIndex<U,K,TPR_m,TPR_M>(this, count);
                    (*hashMap)[K::getOID(newPTR.data)] = new NodeIndex<U,K,TPR_m,TPR_M>(this, count);
                    //LOG(DEBUG) << "[addEntry, Leaf] oid = " << K::getOID(newEntry.data) << ", count/index = " << count << std::endl;
                }
#endif
                count++;
            } else { // need to split the node
                if(TPR_Use_Naive_Split) {
                    //splitNodeNaive(newEntry.mbr.getRefTick(), newEntry, splitNode1, splitNode2);
                    splitNodeNaive(newMBR.getRefTick(), newMBR, newPTR, splitNode1, splitNode2);
                }
                else {
                    //splitNodeRStar(newEntry.mbr.getRefTick(), newEntry, splitNode1, splitNode2);
                    splitNodeRStar(newMBR.getRefTick(), newMBR, newPTR, splitNode1, splitNode2);
                }
                DBUTL_ASSERT((*splitNode1) != NULL);
                DBUTL_ASSERT((*splitNode2) != NULL);
            }


            
        }

        void addEntry(const TPRType& mbr, 
                      U* const ptrData, 
                      TPRTreeNode2D** splitNode1, 
                      TPRTreeNode2D** splitNode2) {
            Ptr ptr;
            ptr.data = ptrData;
            addEntry(mbr, ptr, splitNode1, splitNode2, false
#ifdef USE_NODE_ID
                     , -1
#endif
                );
            //TPRTreeEntry2D newEntry(mbr, ptrData);
            //addEntry(newEntry, splitNode1, splitNode2);
        }

        void addEntry(const TPRType& mbr, 
                      TPRTreeNode2D* childPtr, 
                      TPRTreeNode2D** splitNode1, 
                      TPRTreeNode2D** splitNode2) {

            Ptr ptr;
            ptr.childPtr = childPtr;
            addEntry(mbr, ptr, splitNode1, splitNode2, true
#ifdef USE_NODE_ID
                    , childPtr->id
#endif
               );

            // TPRTreeEntry2D newEntry(mbr, childPtr);
            // addEntry(newEntry, splitNode1, splitNode2);
        }

        void overwriteEntry(const TPRType& mbr, 
                            TPRTreeNode2D* childPtr, 
                            unsigned int index) {
            //TPRTreeEntry2D newEntry(mbr, childPtr);
            
            //Ptr ptr;
            //ptr.childPtr = childPtr;

            mbrs[index] = mbr;
            ptrs[index].childPtr = childPtr;// = ptr;

#ifdef USE_NODE_ID
            eid[index] = childPtr->id;
#endif
            //setIndex(index);

            //entries[index] = newEntry;
            childPtr->index =index;
        }

#ifdef OUTPUT_QMEASURE
        void returnAllPoints(std::vector<U*>* ans, int& node_count) {
            node_count++;
            if (isLeaf()) {
                for (int i = 0; i < count; i++) {                    
                    //ans->push_back(entries[i].data);
                    ans->push_back(ptrs[i].data);
                }
            }
            else {
                for (int i = 0; i < count; i++) {
                    //(entries[i].childPtr)->returnAllPoints(ans, node_count);
                    (ptrs[i].childPtr)->returnAllPoints(ans, node_count);
                }
            }
        }
#endif

        /**
         * a bounding interval must be computed 
         * for the time t_query (given by TPRType* r)
         */
        void search(RType* r, int queryTick, std::vector<U*>* ans) {
            if (isLeaf()) {
                /* S2: Search leaf node. */
                for (int i = 0; i < count; i++) {                    
                    //if (entries[i].mbr.satisfiesQuery(r, queryTick)) {
                    if (mbrs[i].satisfiesQuery(r, queryTick)) {
                        //ans->push_back(entries[i].data);
                        ans->push_back(ptrs[i].data);
                    }
                }
            } else {
                /* S1: Search subtrees recursively. */
                for (int i = 0; i < count; i++) {
                    //if (entries[i].mbr.satisfiesQuery(r, queryTick)) {
                    if (mbrs[i].satisfiesQuery(r, queryTick)) {
                        (ptrs[i].childPtr)->search(r, queryTick, ans);
                    }
                }
            }
        }
        
        /**
         * region is specified by RType* r at the time queryTick
         */
        bool inline intersectsRType(const RType* const left, const RType* const right) const {

#ifdef USE_TPR_EPS
           return   ((left->getLow0()  <= right->getHigh0() + TPR_EPS)
                  && (left->getHigh0() >= right->getLow0()  - TPR_EPS)
                  && (left->getLow1()  <= right->getHigh1() + TPR_EPS)
                  && (left->getHigh1() >= right->getLow1()  - TPR_EPS));

#else
           return   ((left->getLow0()  <= right->getHigh0())
                  && (left->getHigh0() >= right->getLow0())
                  && (left->getLow1()  <= right->getHigh1())
                  && (left->getHigh1() >= right->getLow1()));
#endif
        }

#ifdef OUTPUT_QMEASURE
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r, int queryTick, int& visited_count) {
            visited_count++;
#else
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r, int queryTick) {
#endif


            if (isLeaf()) {



                /* S2: Search leaf node. */
                for (int i = 0; i < count; i++) {

                    //RType futureMBR = entries[i].mbr.getPosRTypeAt(queryTick);
                    RType futureMBR = mbrs[i].getPosRTypeAt(queryTick);

                    if(intersectsRType(r, &futureMBR)) {
                        cb->reportPoint(ptrs[i].data);
                    }
                }
            } else {

                // if(queryTick == 6 && id == 370) {
                //     std::cout << "HERE" << std::endl;
                // }


                /* S1: Search subtrees. */
                for (int i = 0; i < count; i++) {
                    // if(queryTick == 6 && id == 370) {
                    //     std::cout << "Evaluating: " << mbrs[i] << std::endl;
                    //     std::cout << "FutureMBR: " << mbrs[i].getPosRTypeAt(queryTick) << std::endl;
                    //     std::cout << "Query: " << *r << std::endl;
                    // }
                    if (mbrs[i].satisfiesQuery(r, queryTick)) {

                        // if(queryTick == 6 && id == 370) {
                        //     std::cout << "\tSatisfied" << std::endl;
                        // }

                        //RType futureMBR = entries[i].mbr.getPosRTypeAt(queryTick);
                        RType futureMBR = mbrs[i].getPosRTypeAt(queryTick);

                        RType intersection = r->getIntersection(&futureMBR);


                        //(entries[i].childPtr)->enumerateOverRegion(cb, &intersection, queryTick, hash);
#ifdef OUTPUT_QMEASURE
                        //(entries[i].childPtr)->enumerateOverRegion(cb, &intersection, queryTick, visited_count);


                        //(ptrs[i].childPtr)->enumerateOverRegion(cb, &intersection, queryTick, visited_count);
                        (ptrs[i].childPtr)->enumerateOverRegion(cb, r, queryTick, visited_count);
#else
                        //(entries[i].childPtr)->enumerateOverRegion(cb, &intersection, queryTick);
                        //(ptrs[i].childPtr)->enumerateOverRegion(cb, &intersection, queryTick);
                        (ptrs[i].childPtr)->enumerateOverRegion(cb, r, queryTick);
#endif
                    }
                }
            }
        }


        /**
         * Prints a node to out stream using the format
         *    RTreeNode2D(id, level, count, index, {entry1, entry2,...});
         *    if the node is the root then index is set to -1.
         */

        friend std::ostream& operator<<(std::ostream &out, const TPRTreeNode2D<U, K, TPR_m, TPR_M>& node) {

            out << "TPRTreeNode2D(" << std::setiosflags(std::ios::fixed) << std::setprecision(/*TPR_PRECISION*/4)
#ifdef USE_NODE_ID
                << node.id << ", " 
#endif

                << node.level << ", " << node.count  << ", " 
                << node.index << ", " << (node.isLeaf() ? "leaf" : "nonleaf") << ")";
            
#ifdef USE_NODE_ID
            out << ", {";
            if (node.count != 0) {
                out << node.eid[0];
            }
            for (int i = 1; i < node.count; i++) {
                out << ", " << node.eid[i];
            }
            out << "})";
#endif
        }
    };

#ifdef USE_NODE_ID
    template <typename U, typename K, int TPR_m, int TPR_M>
    int TPRTreeNode2D<U, K, TPR_m, TPR_M>::nextID = 0;
#endif

    template <typename U, typename K, int TPR_m, int TPR_M>
    bool TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_Use_Naive_Split = false;

    template <typename U, typename K, int TPR_m, int TPR_M>
    int TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_W = 1;
    template <typename U, typename K, int TPR_m, int TPR_M>
    int TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_U = 59;
    
    // the tree becomes obsolete after TPR_H ticks since the build of the tree.
    template <typename U, typename K, int TPR_m, int TPR_M>
    int TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_H = 110;
    //int TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_H = 60;
    


// #ifdef DELETE_BY_HASH
//     template <typename U, typename K, int TPR_m, int TPR_M>
//     std::tr1::unordered_map<typename K::IDType, typename TPRTreeNode2D<U, K, TPR_m, TPR_M>::NodeIndex<U,K,TPR_m,TPR_M>* >
//     TPRTreeNode2D<U, K, TPR_m, TPR_M>::hashMap;
// #endif



   template<typename U, typename K, typename A, int TPR_m = 1, int TPR_M = 68>
   class TPRTree : public MovingSpatialIndex2D<U, K, A> {

   public:
       //typedef std::tr1::unordered_map<typename K::IDType, NodeIndex<U,K,TPR_m,TPR_M>*> HashType;
       typedef std::unordered_map<typename K::IDType, NodeIndex<U,K,TPR_m,TPR_M>*> HashType;
       TPRTreeNode2D<U, K, TPR_m, TPR_M>* root;
       bool _use_naive_split; // default is false
       bool bulkloadPoints;

        
       int TPR_H;
       int tickNum;
       HashType hashMap;


#ifdef OUTPUT_QMEASURE
        int visit_count, enum_count;
#endif

    public:

       //template<typename U1, typename K1, typename A1, typename RG1, int TPR_m1, int TPR_M1>
       //friend class AENEJoin;

       TPRTreeNode2D<U, K, TPR_m, TPR_M>* getRoot() const {
           return root;
       }
       
        static const char* getClassName() { return "TPRTree"; }
        const char* getInstanceName() { 
            if(_use_naive_split) return "TPRTree_Naive";
            else {
                if(bulkloadPoints) {
                    return "TPRTree_RStar_Bulk";
                }
                else {
                    return "TPRTree_RStar_Insert";
                }
            }
        }
       

       typedef TPRegion<typename K::KeyType0, typename K::KeyType1> TPRType;
       typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;        

        //sowell: Added TPR_H
       TPRTree(int TPR_H = 60, bool use_naive_split = false, bool bulkloadPoints = false) {
           //std::cout << "TPR_M = " << TPR_M << std::endl;
            //LOG(DEBUG) << "[TPR, constructor] " << tickNum << std::endl;


           this->TPR_H = TPR_H;
           TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_H = TPR_H;

            this->bulkloadPoints = bulkloadPoints;
            _use_naive_split = use_naive_split;
            tickNum = 0;
       }
       
       ~TPRTree() {}


       void insertionLoad(std::vector<U*>* agents) {
           TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_Use_Naive_Split = _use_naive_split;

            root = new TPRTreeNode2D<U, K, TPR_m, TPR_M>(&hashMap/*TPR_H*/);
            root->parent = NULL;
           
            for(int i = 0; i < (*agents).size(); i++) {
                RType pos_r(K::getKey0((*agents)[i]), K::getKey0((*agents)[i]),
                            K::getKey1((*agents)[i]), K::getKey1((*agents)[i]));
                RType vel_r(K::getV0((*agents)[i]), K::getV0((*agents)[i]),
                            K::getV1((*agents)[i]), K::getV1((*agents)[i]));
                TPRType r(&pos_r, &vel_r, tickNum);
                
                //LOG(DEBUG) << "[TPR, startTick] insert point: " << *(*agents)[i] << std::endl;
                insert(r, (*agents)[i]);
            }
       }

       void bulkload(std::vector<U*>* agents) {
            TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_Use_Naive_Split = _use_naive_split;

            root = new TPRTreeNode2D<U, K, TPR_m, TPR_M>(&hashMap/*TPR_H*/);
            root->parent = NULL;

//#ifdef BULK_LOAD
            //LOG(DEBUG) << "[BULKLOAD]" << std::endl;
            if(agents->size() > TPR_M) {
                // BULK LOAD
                root->bulkload(agents, tickNum);
            }
            else {
                // INSERTION LOAD
                for(int i = 0; i < (*agents).size(); i++) {
                    RType pos_r(K::getKey0((*agents)[i]), K::getKey0((*agents)[i]),
                                K::getKey1((*agents)[i]), K::getKey1((*agents)[i]));
                    RType vel_r(K::getV0((*agents)[i]), K::getV0((*agents)[i]),
                                K::getV1((*agents)[i]), K::getV1((*agents)[i]));
                    TPRType r(&pos_r, &vel_r, tickNum);
    
                    insert(r, (*agents)[i]);
                }
            }
            //printSubtree(root);
// #else
//                 // INSERTION LOAD
//                 for(int i = 0; i < (*agents).size(); i++) {
//                     RType pos_r(K::getKey0((*agents)[i]), K::getKey0((*agents)[i]),
//                                 K::getKey1((*agents)[i]), K::getKey1((*agents)[i]));
//                     RType vel_r(K::getV0((*agents)[i]), K::getV0((*agents)[i]),
//                                 K::getV1((*agents)[i]), K::getV1((*agents)[i]));
//                     TPRType r(&pos_r, &vel_r, tickNum);
    
//                     //LOG(DEBUG) << "[TPR, startTick] insert point: " << *(*agents)[i] << std::endl;
//                     insert(r, (*agents)[i]);
//                 }
// #endif                       
        }


       /**
        * We assume we always bulkload for queries.
        */
       template<typename RG>
       void bulkloadQueries(std::vector<U*>* agents, RG* rg) {
           TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_Use_Naive_Split = _use_naive_split;

            root = new TPRTreeNode2D<U, K, TPR_m, TPR_M>(&hashMap/*TPR_H*/);
            root->parent = NULL;


            if(agents->size() > TPR_M) {
                // BULK LOAD
                root->bulkloadQueries(agents, tickNum, rg);
            }
            else {
                // INSERTION LOAD
                for(int i = 0; i < (*agents).size(); i++) {


                    U* u = (*agents)[i];
                    RType pos_r = rg->getRegion(u);

                    //std::cout << "pos_r: " << pos_r << std::endl;
                    // RType pos_r(K::getKey0((*agents)[i]), K::getKey0((*agents)[i]),
                    //             K::getKey1((*agents)[i]), K::getKey1((*agents)[i]));


                    RType vel_r(K::getV0(u), K::getV0(u),
                                K::getV1(u), K::getV1(u));
                    TPRType r(&pos_r, &vel_r, tickNum);
    
                    insert(r, u);
                }
            }
        }



        void startTick(std::vector<U*>* agents) {

            //std::cout << "[TPR, startTick] ..... current tick = " << tickNum << std::endl;
            /*          
            //LOG(DEBUG) << "[TPR, startTick] inserting points: " << std::endl;
            for (int i = 0; i < (*agents).size(); i++) {
                //LOG(DEBUG) << "\t\t" << *(*agents)[i] << std::endl;
            }
            */
            
            if(tickNum == 0 || root==NULL) { // we build index only if root is null
                //std::cout << "loading points, tick " << tickNum << std::endl;

                TPRTreeNode2D<U, K, TPR_m, TPR_M>::TPR_H = TPR_H;

                if(bulkloadPoints) {
                    bulkload(agents);
                }
                else {
                    insertionLoad(agents);
                }
                
            }

#ifdef OUTPUT_QMEASURE
            visit_count = 0;
            enum_count = 0;
#endif
            
            //LOG(DEBUG) << "[TPR, startTick]" << " end of startTick" << std::endl;

            //printSubtree(root);
        }

       template<typename RG>
       void startTickQueries(std::vector<U*>* queryAgents, RG* rg) {
           if(tickNum == 0 || root==NULL) { // we build index only if root is null

               //std::cout << "HERHERHERHERHERHERHERERHERHERHEHRER" << std::endl;
               bulkloadQueries(queryAgents, rg);
               //std::cout << "HERHERHERHERHERHERHERERHERHERHEHRER2" << std::endl;               
           }

#ifdef OUTPUT_QMEASURE
           visit_count = 0;
           enum_count = 0;
#endif
       }








        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r) {
            DBUTL_ASSERT ( root != NULL );


            // if(tickNum == 6)
            //     std::cout << "enumerateOverRegion: " << *r << std::endl;
            
            //if(tickNum == 9) 
                ///*LOG(DEBUG)*/std::cout << "[TPR, enumOverRegion] enumerating region " << *r << std::endl;


            //LOG(DEBUG) << "[TPR, enumOverRegion] the tree contains:" << std::endl;
            //printSubtree(root);


            //NOTE: can use any tick no less than tickNum for future queries
#ifdef OUTPUT_QMEASURE
            int visited_count = 0; // Quality Measure
            root->enumerateOverRegion(cb, r, tickNum, visited_count);



            visit_count += visited_count;
            enum_count++;
#else

            root->enumerateOverRegion(cb, r, tickNum);

#endif
        }

        void aggregateOverRegion(A* aggs, RType* r) {
            //LOG(DEBUG) << "[TPR, aggregateOverRegion]" << std::endl; 

            aggs->reset();

            std::vector<U*> ans;

            root->search(r, tickNum, &ans);

            typename std::vector<U*>::const_iterator iter;
            for(iter = ans.begin(); iter != ans.end(); iter++) {
                aggs->combineFromOne(*iter);
            }
        }

        void nearestK(IndexCallBack<U>* cb, U* u, int k) {
        }

        void afterUpdateAgg(U* u) { 
        }

        void beforeDelete(U* u) {
           //LOG(DEBUG) << "[TPR, beforeDelete] " << *u << std::endl; 
#ifdef REBUILD_EVERY_TICK
#else            
           deleteData(u);
#endif
        }

        void afterInsert(U* u) {
            //LOG(DEBUG) << "[TPR, afterInsert] " << *u << std::endl; 
            RType pos_r(K::getKey0(u), K::getKey0(u),
                        K::getKey1(u), K::getKey1(u));
            RType vel_r(K::getV0(u), K::getV0(u),
                        K::getV1(u), K::getV1(u));

            // update is valid for the beginning of the next tick
            TPRType r(&pos_r, &vel_r, tickNum + 1);

            //std::cout << "[TPR, afterInsert] insert point: " << (*u) << std::endl;
            insert(r, u);

        }
       template<typename RG>
       void afterInsertQuery(U* u, RG* rg) {
           RType pos_r = rg->getRegion(u);

            //LOG(DEBUG) << "[TPR, afterInsert] " << *u << std::endl; 
            // RType pos_r(K::getKey0(u), K::getKey0(u),
            //             K::getKey1(u), K::getKey1(u));
            RType vel_r(K::getV0(u), K::getV0(u),
                        K::getV1(u), K::getV1(u));

            // update is valid for the beginning of the next tick
            TPRType r(&pos_r, &vel_r, tickNum + 1);

            //std::cout << "[TPR, afterInsert] insert point: " << (*u) << std::endl;
            insert(r, u);

        }




        void beforeUpdateVelocity(U* point, 
                                  typename K::KeyType0 newP0, 
                                  typename K::KeyType1 newP1, 
                                  typename K::KeyType0 newV0, 
                                  typename K::KeyType1 newV1) {
            //LOG(DEBUG) << "[TPR, beforeUpdateVel] tick = " << tickNum                        << ", point: " << *point                       << ", newP0 = " << newP0 << ", newP1 = " << newP1                       << ", newV0 = " << newV0 << ", newV1 = " << newV1 << std::endl;
                        
#ifdef REBUILD_EVERY_TICK
#else            
            deleteData(point);

            RType pos_r(newP0, newP0, newP1, newP1);
            RType vel_r(newV0, newV0, newV1, newV1);

            // update is valid for the beginning of the next tick
            TPRType r(&pos_r, &vel_r, tickNum + 1); 

            //LOG(DEBUG) << "[TPR, beforeUpdateVel] insert point: " << (*point) << std::endl; 
            insert(r, point);

            //LOG(DEBUG) << "[TPR, beforeUpdateVel] after updating velocity: " << std::endl; 
#endif
            
        }


        void endTick(bool force = false) {             
#ifdef OUTPUT_QMEASURE
            LOG(INFO) << "[TPR, endTick] tick = " << tickNum << std::endl;

            if(enum_count > 0) {
                std::vector<U*> tmp_vec;
                int node_count = 0;
                root->returnAllPoints(&tmp_vec, node_count);
                LOG(INFO) << "[TPR, endTick] visit count = " << visit_count << " average = " << ((double) visit_count / enum_count) << " total # of nodes = " << node_count << " total # of data points = " << tmp_vec.size() << std::endl;
            }
#endif

#ifdef REBUILD_EVERY_TICK
            if(root != NULL) {
                deleteSubtreeRecursively(root);
#ifdef USE_NODE_ID
                TPRTreeNode2D<U, K, TPR_m, TPR_M>::nextID = 0;
#endif
                root = NULL;
            }
#else
            if(force || (tickNum+1) % TPR_H == 0) {
                //LOG(DEBUG) << "[TPR, endTick] force = true, we delete the tree" << std::endl;
                if(root != NULL) {
                    deleteSubtreeRecursively(root);
#ifdef USE_NODE_ID
                    TPRTreeNode2D<U, K, TPR_m, TPR_M>::nextID = 0;
#endif
                    root = NULL;
                    tickNum = -1;
                }

                


#ifdef DELETE_BY_HASH
                typename HashType::iterator it;
//                typename std::tr1::unordered_map<typename K::IDType, typename TPRTreeNode2D<U, K, TPR_m, TPR_M>::NodeIndex<U,K,TPR_m,TPR_M>* >::iterator iter = TPRTreeNode2D<U, K, TPR_m, TPR_M>::hashMap.begin();
                for(it = hashMap.begin(); it != hashMap.end(); it++) {
                    delete it->second;
                }
                hashMap.clear();
                //TPRTreeNode2D<U, K, TPR_m, TPR_M>::hashMap.clear();
#endif
            }
#endif            

            tickNum++;
        }

    private:
        void deleteData(U* point) {
            //std::cout << "[TPR, deleteData] current tick = " << tickNum << ", point: " << (*point) << std::endl;
            //LOG(DEBUG) << "[TPR, deleteData] Before Delete: " << std::endl;
            //printSubtree(root);
            bool res;
#ifdef DELETE_BY_HASH

            res = root->deleteDataByOID(K::getOID(point));
#else

           
#ifdef USE_TPR_EPS
            RType pos_r(K::getKey0(point) - TPR_EPS, K::getKey0(point) + TPR_EPS,
                        K::getKey1(point) - TPR_EPS, K::getKey1(point) + TPR_EPS);
#else
            RType pos_r(K::getKey0(point), K::getKey0(point),
                        K::getKey1(point), K::getKey1(point));
#endif
            //LOG(DEBUG) << "\t\t\t looking for: " << pos_r << std::endl;

            res = root->searchToDelete(tickNum + 1, &pos_r, point);
#endif

            //if(!res) {
            //LOG(DEBUG) << "[TPR, deleteData] point not found: " << (*point) << std::endl;
            //DBUTL_ASSERT( false );
            //}

            //LOG(DEBUG) << "[TPR, deleteData] After Delete: " << std::endl;
            //printSubtree(root);
            //LOG(DEBUG) << "[TPR, deleteData] ------------------------------ " << std::endl;
            //verifyTree(root);

        }

        /**
         * delete the subtree rooted at node (free the allocated memory)
         */
        void deleteSubtreeRecursively(TPRTreeNode2D<U, K, TPR_m, TPR_M>* node) {
            DBUTL_ASSERT(node != NULL);
            if (node->isLeaf()) {
                delete node;
                return;
            }
            for (int i = 0; i < node->count; i++) {
                //deleteSubtreeRecursively((node->entries[i]).childPtr);
                deleteSubtreeRecursively((node->ptrs[i]).childPtr);
            }
            delete node;
        }

        TPRTreeNode2D<U, K, TPR_m, TPR_M>* chooseLeaf(TPRTreeNode2D<U, K, TPR_m, TPR_M>* node, 
                                                      const TPRType& mbr) {
            if (node->isLeaf()) {
                return node;
            } else {
                TPRTreeNode2D<U, K, TPR_m, TPR_M>* childNode = node->chooseSubTree(mbr);
                return chooseLeaf(childNode, mbr);
            }
        }

        /**
         * Prints a tree using the format:
         *   node:
         *       child1 <---> child1's mbr
         *       child2 <---> child2's mbr
         *       ...
         *   child1: child11, child12, ...
         *   child2: child21, child22, ...
         *   .
         *   .
         *   .
         *   child11: child111, child112, ...
         *   .
         *   .
         *   .
         *        --------------------------
         *   leaf1: mbr11, mbr12,...
         *   leaf2: mbr21, mbr22,...
         *   .
         *   .
         *   .
         */

   public: //TEMP: for testing
         
        void printSubtree(TPRTreeNode2D<U, K, TPR_m, TPR_M>* node) {
            if (node->isLeaf()) {
                /*LOG(DEBUG)*/std::cout << "\tLeaf: " << *node << ": " << std::endl;
                for (int i = 0; i < node->count; i++) {
                    /*LOG(DEBUG)*/std::cout<< "\t\t\t" << *((node->ptrs[i]).data) << std::endl;
                    /*LOG(DEBUG)*/std::cout << "\t\t\t" << (node->mbrs[i]) << std::endl;
                }
                return;
            }

            /*LOG(DEBUG)*/std::cout << "\t" << *node << ": " << std::endl;

            std::vector<TPRTreeNode2D<U, K, TPR_m, TPR_M>* > nodes;
            for (int i = 0; i < node->count; i++) {
                //TPRTreeNode2D<U, K, TPR_m, TPR_M>* child = (node->entries[i]).childPtr;
                TPRTreeNode2D<U, K, TPR_m, TPR_M>* child = (node->ptrs[i]).childPtr;

                /*LOG(DEBUG)*/std::cout << "\t\t [" << child << "]";

                nodes.push_back(child);                

                /*LOG(DEBUG)*/std::cout << "\t\t" << *child << "<--->" << node->mbrs[i] << std::endl;
            }

            /*LOG(DEBUG)*/std::cout << "---------------------------------------------------"                       << std::endl;

            typename std::vector<TPRTreeNode2D<U, K, TPR_m, TPR_M>* >::const_iterator iter;
            for(iter = nodes.begin(); iter != nodes.end(); iter++) {
                printSubtree(*iter);
            }
        }
            

        /** //verify if invariants are kept within the subtree
         *  whose root is given by 'node'.
         */
        bool verifyTree(TPRTreeNode2D<U, K, TPR_m, TPR_M>* node) {
            if (node->isLeaf()) {
                if(node->level != 0) {
                    std::cout << "\t Leaf: " << *node << " has wrong level value" << std::endl;
                    return true;
                }
                return false;
            }

            std::vector<TPRTreeNode2D<U, K, TPR_m, TPR_M>* > nodes;
            for (int i = 0; i < node->count; i++) {
                //TPRTreeNode2D<U, K, TPR_m, TPR_M>* child = (node->entries[i]).childPtr;
                TPRTreeNode2D<U, K, TPR_m, TPR_M>* child = (node->ptrs[i]).childPtr;
                if(child->index != i) {
                    std::cout << "\tChild: " << *child << " has wrong index (exp: "
                              << i << " got: " << child->index << ")" << std::endl;
                }
                if(child->level != node->level - 1) {
                    std::cout << "\tChild: " << *child << " has wrong level (exp: " 
                              << node->level-1 << " got: " << child->level << ")" << std::endl;
                }
                if(child->parent != node) {
                    std::cout << "\tChild: " << *child << " has wrong parent (exp: [" 
                              << node << "] " << *node << ",  got: [" << child->parent 
                              << "] " << *(child->parent) << ")" << std::endl;
                }
                nodes.push_back(child);
            }

            typename std::vector<TPRTreeNode2D<U, K, TPR_m, TPR_M>* >::const_iterator iter;
            bool ct = false;
            for(iter = nodes.begin(); iter != nodes.end(); iter++) {
                ct |= verifyTree(*iter);
            }
            if(node->isRoot())
                DBUTL_ASSERT(!ct);
            return ct;
        }
        

        /* Algorithm Search:*/
        std::vector<U*> search(const RType& r, int queryTick) {
            std::vector<U*> ans;
            root->search(r, queryTick, &ans);
        }

        void deleteEntry() { 
            // No need to implement
        }

        /* Algorithm Insert:
         * Insert a new index entry into the TPRTree 
         * Current tick is specified by mbr.getRefTick() [equal to tickNnum]
         */
        void insert(TPRType mbr, U* const ptrData) {
            
            ///*LOG(DEBUG)*/std::cout << "[TPR, insert] MBR: " << mbr << std::endl;            
            
            /* I1: invoke chooseLeaf to select a leaf node to insert a new entry. */
            /* I2: If the leafNode has room, insert new entry; otherwise, invoke splitNode */
            /* I3: adjustTree (propagates upward to the root)  */
            TPRTreeNode2D<U, K, TPR_m, TPR_M>* splitNode1 = NULL;
            TPRTreeNode2D<U, K, TPR_m, TPR_M>* splitNode2 = NULL;
            



            root->chooseLeafToInsert(mbr, ptrData, &splitNode1, &splitNode2);

            ///*LOG(DEBUG)*/std::cout << "[TPR, insert] calling adjustTree on root " << std::endl;

            root->adjustTree(mbr.getRefTick(), &splitNode1, &splitNode2);
            
            LOG(DEBUG) << "[TPR, insert] at the end of insert:: " << std::endl;
            LOG(DEBUG) << "[TPR, insert] --------------------------------------- " << std::endl;

            //verifyTree(root);
            return;
        }

    };



   template<typename U, typename K, typename A>
   class TPRTreeDefault : public TPRTree<U, K, A, 1, 68> {
   };

}


#endif /* _TPRTREE_2D_H_ */

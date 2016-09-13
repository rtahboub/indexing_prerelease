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


#ifndef _SIMPLEGRID_H_
#define _SIMPLEGRID_H_

#include <map>
#include <set>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
//#include <tr1/unordered_map>
#include <unordered_map>
#include "2D/StaticSpatialIndex2D.h"
#include "util/AlignedMem.h"

namespace PSimIndex {
    template<typename U, typename K, typename A>
    class SimpleGrid : public StaticSpatialIndex2D<U, K, A> {

    //------------------------------------------------------------------------
    // Static Constants
    //------------------------------------------------------------------------

        static const uint32_t DEFAULT_BUCKET_SIZE = 4;

    //------------------------------------------------------------------------
    // Class Declarations
    //------------------------------------------------------------------------

    private:

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;


        struct Entry {
            Entry* next;
            Entry* prev;
            U* data;
        };

        class Bucket { 
        public:
        
            static uint32_t bucketSize;

        private:
            int num_points;
            Entry* _head;
            Entry* _tail;
            Bucket* _bucketNext;
        public:
            typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;
            
            Bucket();
            Bucket(Bucket* bucketNext); 
            ~Bucket() {}
            void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);
            // add all points to the callback
            // because the grid is fully covered in the region
            void addAllPoints(IndexCallBack<U>* cb);
            // recursively delete the list. 
            // if the next bucket is non-empty, recursively delete it first
            // and then delete the list. 
            void deleteRec();
            bool isFull() { return (num_points == bucketSize); }
            void insert(U* data);
        };


        class GridCell {
        private:
            int num_points;
        public:

            Bucket* bucketHead;

            GridCell();
            ~GridCell() {}

            void insert(U* data);
            void clear();
            void deleteRec();
            void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);
            void addAllPoints(IndexCallBack<U>* cb);
        };


    //------------------------------------------------------------------------
    // Private Members
    //------------------------------------------------------------------------
    private:
        GridCell* cells;
        int tickNum;
        int spaceSide;
        double cellSize;
        int cellsPerSide;
        bool setCellsPerSide;

        inline GridCell* getCell(int x, int y) {
            return &cells[x * cellsPerSide + y];
        }

    //------------------------------------------------------------------------
    // Public Members
    //------------------------------------------------------------------------

    public:

        static const char* getClassName() { return "SimpleGrid"; }
        const char* getInstanceName() { return "SimpleGrid"; }

        SimpleGrid(int cellsPerSide = -1, 
                   int spaceSide = 22361, 
                   uint32_t bucketSize = DEFAULT_BUCKET_SIZE,
                   int numAgents = 50000);

        ~SimpleGrid();
        void startTick(std::vector<U*>* agents);
        void enumerateOverRegion(IndexCallBack<U>* cb, RType* r);
        void endTick(bool force = false);


        // Unimplemented Methods.
        void aggregateOverRegion(A* aggs, RType* r) {}
        void nearestK(IndexCallBack<U>* cb, U* u, int k) {}
        void afterUpdateAgg(U* u) {}
        void beforeDelete(U* u) {}
        void afterInsert(U* u) {}
    };



    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Implementation Starts Here
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------


    //------------------------------------------------------------------------
    // Bucket Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    uint32_t SimpleGrid<U,K,A>::Bucket::bucketSize = DEFAULT_BUCKET_SIZE;

    template<typename U, typename K, typename A>
    SimpleGrid<U,K,A>::Bucket::Bucket() {
        _bucketNext = NULL;
        num_points = 0;
        _head = _tail = NULL;
    }

    template<typename U, typename K, typename A>
    SimpleGrid<U,K,A>::Bucket::Bucket(Bucket* bucketNext) {
        _bucketNext = bucketNext;
        num_points = 0;
        _head = _tail = NULL;
    }

    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::Bucket::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                        RType* r) {
        Entry* node = _head;

        while(node != NULL) {
            if(r->template containsPoint<U, K>(node->data)) {
                LOG(DEBUG) << "[Grid enumOverRegion] reporting: "
                           << *(node->data) << std::endl; 

                cb->reportPoint(node->data);
            }
            node = node->next;
        }

        if(_bucketNext != NULL) {
            _bucketNext->enumerateOverRegion(cb, r);
        }
    }


    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::Bucket::addAllPoints(IndexCallBack<U>* cb) {
        Entry* node = _head;
        while(node != NULL) {
            cb->reportPoint(node->data);
            node = node->next;
        }

        if(_bucketNext != NULL) {
            _bucketNext->addAllPoints(cb);
        }
    } 


    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::Bucket::deleteRec() {
        if(_bucketNext != NULL) { 
            _bucketNext->deleteRec();
            delete _bucketNext;
            _bucketNext = NULL;
        }


        Entry* node = _head;
        Entry* next;
        while(node != NULL) {
            next = node->next;
            delete node;
            node = next;
        }
    }


    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::Bucket::insert(U* data) {
        DBUTL_ASSERT(num_points + 1 <= bucketSize);
            
        Entry* new_entry = new Entry();

        new_entry->prev = NULL;
        new_entry->next = _head;
        new_entry->data = data;
        if(_head != NULL)
            _head->prev = new_entry;
        if(_tail == NULL) 
            _tail = _head;
        _head = new_entry;

        num_points++;
    }

    //------------------------------------------------------------------------
    // GridCell Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    SimpleGrid<U,K,A>::GridCell::GridCell() {
        num_points = 0;
        bucketHead = new Bucket();
    }

    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::GridCell::insert(U* data) {
        DBUTL_ASSERT((bucketHead != NULL));

        if(bucketHead -> isFull()) {
            Bucket* newHead = new Bucket(bucketHead);
            bucketHead = newHead;
        }
        bucketHead->insert(data);
        num_points++;
    }

    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::GridCell::clear() {
        deleteRec();
        num_points = 0;
        bucketHead = new Bucket();            
    }

    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::GridCell::deleteRec() {
        if(bucketHead != NULL) {
            bucketHead->deleteRec();
            delete bucketHead;
            bucketHead = NULL;
        }
    }

    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::GridCell::enumerateOverRegion(IndexCallBack<U>* cb,
                                                          RType* r) {
        if(bucketHead != NULL) {
            bucketHead->enumerateOverRegion(cb, r);
        }
    }

    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::GridCell::addAllPoints(IndexCallBack<U>* cb) {
        if(bucketHead != NULL) {
            bucketHead->addAllPoints(cb);
        }
    }


    //------------------------------------------------------------------------
    // Public Methods
    //------------------------------------------------------------------------

    template<typename U, typename K, typename A>
    SimpleGrid<U,K,A>::SimpleGrid(int cellsPerSide,
                                  int spaceSide,
                                  uint32_t bucketSize,
                                  int numAgents) {
        this->spaceSide = spaceSide;            

        //Use fixed number of cells per side. In this case numAgents is ignored. 
        if(cellsPerSide != -1) {
            this->cellsPerSide = cellsPerSide;
        }
        //Use formula based on the number of agents. Note that this means
        //we can't use a single SimpleGrid instance on multiple workloads,
        //but this is true due to spaceSide anyway. 
        else {
            double numCells = numAgents / 296.0;
            this->cellsPerSide = (int) std::ceil(std::sqrt(numCells));
            //std::cout << "numCells " << numCells << " cellsPerSide = " << this->cellsPerSide;
        }



        cells = (GridCell*) AlignedMem::allocateClear((sizeof(GridCell) 
                                                            * this->cellsPerSide 
                                                            * this->cellsPerSide),
                                                           64);

        for(int i = 0; i < this->cellsPerSide; i++) {
            for(int j = 0; j < this->cellsPerSide; j++) {
                getCell(i,j)->clear();
            }
        }

        cellSize = (double) spaceSide / (double) this->cellsPerSide;
        //std::cout << " cellSize " << cellSize << std::endl;
        tickNum = 0;

        Bucket::bucketSize = bucketSize;
    }


    template<typename U, typename K, typename A>
    SimpleGrid<U,K,A>::~SimpleGrid() {
        for(int i = 0; i < cellsPerSide; i++) {
            for(int j = 0; j < cellsPerSide; j++) {
                getCell(i,j)->deleteRec();

            }
        }
        AlignedMem::deallocate(cells);
    }





    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::startTick(std::vector<U*>* agents) {
        typename std::vector<U*>::iterator it;

        for(int i = 0; i < agents->size(); ++i) {
            int idx_x = (int) (K::getKey0((*agents)[i]) / cellSize);
            int idx_y = (int) (K::getKey1((*agents)[i]) / cellSize);

            DBUTL_ASSERT( 0 <= idx_x && idx_x < cellsPerSide);
            DBUTL_ASSERT( 0 <= idx_y && idx_y < cellsPerSide);
            getCell(idx_x, idx_y)->insert(agents->at(i));
        }
    }


    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::enumerateOverRegion(IndexCallBack<U>* cb, 
                                                RType* r) {
    	//prune disqualified grid cells
    	int x1, x2, y1, y2;
    	x1 = r->getLow0()/cellSize;
    	x2 = r->getHigh0()/cellSize;
    	y1 = r->getLow1()/cellSize;
    	y2 = r->getHigh1()/cellSize;

    	if(x1 == x2 && y1 == y2){

    		RType* rect = new RType(cellSize*x1, cellSize*(x1+1), cellSize*y1, cellSize*(y1+1));

    		if(r->contains(rect)) {
				// fully covered
				getCell(x1,y1)->addAllPoints(cb);
    		} else if(r->intersects(rect)) {
    			getCell(x1,y1)->enumerateOverRegion(cb, r);
    		}
    	delete rect;
    	DBUTL_ASSERT(getCell(x1,y1) != NULL);

    	}else if(x1 == x2 && y2 > y1){
    		for (int i=y1; i<=y2; i++){
    			RType* rect = new RType(cellSize*x1, cellSize*(x1+1), cellSize*i, cellSize*(i+1));
    			if(r->contains(rect)) {
    			// fully covered
    			getCell(x1,i)->addAllPoints(cb);
    			} else if(r->intersects(rect)) {
    				getCell(x1,i)->enumerateOverRegion(cb, r);
    			}
    			delete rect;
    			DBUTL_ASSERT(getCell(x1,i) != NULL);
    		}
    	}

    	else if(y1 == y2 && x2 > x1){
    		for(int i=x1; i<=x2; i++){
    			RType* rect = new RType(cellSize*i, cellSize*(i+1), cellSize*y1, cellSize*(y1+1));
    			if(r->contains(rect)) {
					// fully covered
					getCell(i,y1)->addAllPoints(cb);
				} else if(r->intersects(rect)) {
					getCell(i,y1)->enumerateOverRegion(cb, r);
    			}
    			delete rect;
    			DBUTL_ASSERT(getCell(i,y1) != NULL);
    		}
    	}
    	else{

			for(int i = x1; i <= x2-x1; i++){
				for (int j = y1; j <= y2-y1; j++){
					RType* rect = new RType(cellSize*i, cellSize*(i+1), cellSize*j, cellSize*(j+1));

						if(r->contains(rect)) {
							// fully covered
							getCell(i,j)->addAllPoints(cb);
						} else if(r->intersects(rect)) {

							getCell(i,j)->enumerateOverRegion(cb, r);
						}
						delete rect;
						DBUTL_ASSERT(getCell(i,j) != NULL);
				}

			}
    	}

//        for(int i = 0; i < cellsPerSide; ++i) {
//            for(int j = 0; j < cellsPerSide; ++j) {
//
//                //TODO: Avoid allocation here.
//                RType* rect = new RType(cellSize*i, cellSize*(i+1), cellSize*j, cellSize*(j+1));
//
//                if(r->contains(rect)) {
//    //            	std::cout<<"contains " << i << " " << j << " " << *rect << " " << *r << std::endl;
//                    // fully covered
//                    getCell(i,j)->addAllPoints(cb);
//                }
//                else if(r->intersects(rect)) {
////                	std::cout<<"intersects " << i << " " << j << " " << *rect << " " << *r << std::endl;
//                    getCell(i,j)->enumerateOverRegion(cb, r);
//                }
//                delete rect;
//                DBUTL_ASSERT(getCell(i,j) != NULL);
//            }
//        }
    }


    template<typename U, typename K, typename A>
    void SimpleGrid<U,K,A>::endTick(bool force) {
        for(int i = 0; i < cellsPerSide; i++) {
            for(int j = 0; j < cellsPerSide; j++) {
                getCell(i,j)->clear();
            }
        }

        tickNum++;

        if(force) {
            //AlignedMem::deallocate(cells); TODO?? 
            tickNum = 0;
        }
    }
}


#endif



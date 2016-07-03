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

#ifndef _WORKLOAD_2D_H_
#define _WORKLOAD_2D_H_

#include <vector>
#include <iostream>
#include "2D/TestTypes2D.h"
#include "2D/Region2D.h"


namespace PSimIndex {

    /**
     * Type of query. 
     */
    enum QueryType {
        ENUM = 0, 
        AGG  = 1, 
        KNN  = 2
    };

    /**
     * Type of update. 
     */
    enum UpdateType {
        VEL_UPDATE = 0, 
        AGG_UPDATE = 1, 
        POS_UPDATE = 2, 
        INSERT     = 3, 
        DELETE     = 4,
        IMPLICIT_MOVE = 5 // FOR INTERNAL USE ONLY
    };

    /**
     *
     */
    struct QueryRec {
        QueryType type;
        std::vector<Point2D*>* queryPoints;
        RegionGen2D* regionGen;

        //For knn queries.  
        int k;

        QueryRec(QueryType t, std::vector<Point2D*>* q, RegionGen2D* r) 
            : type(t), queryPoints(q), regionGen(r), k(0) {}

        QueryRec(QueryType t, 
                 std::vector<Point2D*>* q, 
                 RegionGen2D* r, 
                 int k) 
            : type(t), queryPoints(q), regionGen(r), k(k) {}


        ~QueryRec() {
            if(queryPoints != 0) {
                queryPoints->clear();
                delete queryPoints;
            }
            delete regionGen;
        }
    };

    /**
     *
     *
     *
     */
    struct UpdateRec {
        UpdateType type;
        Point2D* point;
        double newX, newY, newVx, newVy, newAgg;

        UpdateRec(UpdateType t, Point2D* p) : type(t), point(p) {}
        UpdateRec(UpdateType t, Point2D* p, double nx, double ny, 
                  double nvx, double nvy)
            : type(t), point(p), newX(nx), newY(ny), newVx(nvx), newVy(nvy)
        {}
        UpdateRec(UpdateType t, Point2D* p, double na)
            : type(t), point(p), newAgg(na) {}
    };


    /**
     *
     *
     */
    std::ostream& operator<<(std::ostream &out, const QueryRec& q);  


    /**
     *
     */
    std::ostream& operator<<(std::ostream &out, const UpdateRec& u);


    /**
     * Represents a spatial join workload. During each 
     *
     */
    class Workload2D {
    public:
        
        virtual char* getName() = 0;

        virtual int getSize() = 0;
        
        virtual void insertPoint(Point2D* point) = 0;
        virtual bool deletePoint(Point2D* point) = 0;
//        virtual void updateVelocity(UpdateRec* update) = 0;



        /**
         * Returns true iff there are more ticks. This should be called before
         * every tick.
         */
        virtual bool hasNextTick() = 0;


        virtual int getTickNum() = 0;

        virtual void startTick() = 0;

        /**
         * Returns the set of points to index. This should be called every
         * tick for building.
         */
        virtual std::vector<Point2D*>* getPoints() = 0;


        // Note: We use Java style iterators here instead of C++ style
        // iterators because we need to do some processing when returning each
        // update. This is considerably easier to implement when we have a
        // method call per object. 


        virtual bool hasNextQuery() = 0;
        virtual QueryRec* nextQuery() = 0;

        virtual bool hasNextUpdate() = 0;
        virtual UpdateRec* nextUpdate() = 0;


        /**
         *
         *
         */
        virtual void endTick() = 0;


        // Note: these methods should only be used between ticks.
        // virtual void saveState() = 0;
        // virtual void resetState() = 0;
        // virtual void deleteState() = 0;

        /**
         * Resets this iterator to tick 1. A workload should return the exact
         * same queries and updates each time it is reset. 
         */
        virtual void reset() = 0;


        virtual void printTest() = 0;
        virtual ~Workload2D(){}


    };
}

#endif /* _WORKLOAD_2D_H_ */

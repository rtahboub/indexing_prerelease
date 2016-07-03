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

#ifndef _TRACE_WORKLOAD_2D_H_
#define _TRACE_WORKLOAD_2D_H_

#include <iostream>
#include <cstdio>
#include <vector>
#include <list>
//#include <map>
//#include <tr1/unordered_map>
//#include <tr1/unordered_set>
#include <unordered_map>
#include <unordered_set>

#include "2D/TestTypes2D.h"
#include "2D/Workload2D.h"

#include "util/Log.h"


namespace PSimIndex {

    class TraceWorkload2D : public Workload2D {

    private:
        static const int BUFFSIZE = 10485760;
        FILE* traceFile;
        char fileBuffer[BUFFSIZE];
        bool isBinary;
        const char* filename;

      //  std::tr1::unordered_map<int, Point2D*>* pointMap;
       // std::tr1::unordered_set<int> pointsPosMove;

        std::unordered_map<int, Point2D*>* pointMap;
        std::unordered_set<int> pointsPosMove;

        std::vector<QueryRec*>* queries;
        std::vector<UpdateRec*>* updates;

        


        // Records the position in the file corresponding to the beginning of
        // the next tick. This will allow us to "replay" ticks. 
        //fpos_t filePos;
        //std::tr1::unordered_map<int, Point2D*>* pointMapCopy;
        //std::list<Point2D*> toDelete;
        


        //bool isReplayable;
        
        int numTicks, currTick;
        int queryCount, updateCount;

        char name[200];
        int size;

        QueryRec* processQuery(int type,
                               std::vector<Point2D*>* queryPoints,
                               double xDelta,
                               double yDelta,
                               int k);

        UpdateRec* processUpdate(int type,
                                 int id,
                                 double x,
                                 double y,
                                 double vx,
                                 double vy,
                                 double data);


        void parseHeaderText();
        void parseTickText();

        void parseHeaderBinary();
        void parseTickBinary();

        std::vector<Point2D*>* toVector();

//        void deepCopyPoints();

        void deletePoints();
        inline void clearQueries();
        inline void clearUpdates();
        
        //Testing
        void printMap();


    public:
        char* getName();
        int getSize();


        const char* getFileName();
        bool getBinary();

        //bool getReplayable();
        //void setReplayable(bool replayable);

        void insertPoint(Point2D* point);
        bool deletePoint(Point2D* point);


        int getTickNum();

        bool hasNextTick();

        void startTick();

        std::vector<Point2D*>* getPoints();

        bool hasNextQuery();
        QueryRec* nextQuery();

        bool hasNextUpdate();
        UpdateRec* nextUpdate();

        void printTest() {
            for(int j = 0; j < queries->size(); j++) {
                QueryRec* query = queries->at(j);

                for(int i = 0; i < query->queryPoints->size(); i++){
                    std::cout << query->queryPoints->at(i) << " ";
                }
            }
            std::cout << std::endl;
        }

        void endTick();


        // Note: these methods should only be used between ticks.
        //void saveState();
        //void resetState();
        //void deleteState();

        void reset();

        TraceWorkload2D(const char* const filename, 
                        bool binary/*, 
                                     bool replayable = true*/);
        ~TraceWorkload2D();
    };


    // -------------------------------
    // Inline members defined here.
    // -------------------------------


    inline char* TraceWorkload2D::getName() {
        return name;
    }
    
    inline int TraceWorkload2D::getSize() {
        return size;
    }

    inline int TraceWorkload2D::getTickNum() {
        return currTick;
    }

    inline const char* TraceWorkload2D::getFileName() {
        return filename; 
    }
    inline bool TraceWorkload2D::getBinary() {
        return isBinary;
    }


    // inline bool TraceWorkload2D::getReplayable() { 
    //     return isReplayable; 
    // }

    // inline void TraceWorkload2D::setReplayable(bool replayable) { 
    //     isReplayable = replayable;
    // }

    inline void TraceWorkload2D::clearQueries() {
        for(int i = 0; i < queries->size(); i++) {
            delete queries->at(i);
        }
        delete queries;
        queries = 0;
    }

    inline void TraceWorkload2D::clearUpdates() {
        for(int i = 0; i < updates->size(); i++) {
            delete updates->at(i);
        }
        delete updates;
        updates = 0;
    }

    inline void TraceWorkload2D::insertPoint(Point2D* point) {
        DBUTL_ASSERT(pointMap->count(point->id) == 0);
        //toInsert.push_back(point);
        (*pointMap)[point->id] = point;
    }


    inline bool TraceWorkload2D::deletePoint(Point2D* point) {
        //toDelete.push_back(point);
        //std::cout << "Deleting: " << *point << " "; //std::endl;




        bool ret = ((pointMap->erase(point->id) == 1) ? true : false);

        // if(ret == true) {
        //     std::cout << " true" << std::endl;
        // }
        // else {
        //     std::cout << " false" << std::endl;
        // }


        // std::cout << "Point Map:" << std::endl;
        // std::tr1::unordered_map<int,Point2D*>::iterator it;
        // for(it = pointMap->begin(); it != pointMap->end(); it++) {
        //     std::cout << it->first << " " << *it->second << std::endl;
        // }





        if(ret) { delete point; }
        //if(ret) { toDelete.push_back(point); }
        return ret;
    }

    //inline void TraceWorkload2D::updateVelocity(UpdateRec* update) {
    //toUpdate.push_back(update);
    //}

    inline std::vector<Point2D*>* TraceWorkload2D::getPoints() {
        return toVector();
    }

    inline bool TraceWorkload2D::hasNextTick() {
        return (currTick < numTicks);
    }

    inline bool TraceWorkload2D::hasNextQuery() {
        return (queryCount < queries->size());
    }

    inline QueryRec* TraceWorkload2D::nextQuery() {

    // for(int j = 0; j < queries->size(); j++) {
    //     QueryRec* query = queries->at(j);

    //     for(int i = 0; i < query->queryPoints->size(); i++){
    //         LOG(DEBUG) << "Query Point 1.5 "
    //                    << query->queryPoints->at(i)
    //                    << std::endl;
    //     }
    // }
     //   std::cout << "queryCount: "  << queryCount << " queries->size(): " << queries->size() << std::endl;

        QueryRec* rec = (*queries)[queryCount];
        queryCount++;
        return rec;
    }

    inline bool TraceWorkload2D::hasNextUpdate() {
        return (updateCount < updates->size());
    }

    inline UpdateRec* TraceWorkload2D::nextUpdate() {
        return updates->at(updateCount++);
    }


    // inline void TraceWorkload2D::deleteState() {
    //     std::list<Point2D*>::iterator it;
    //     for(it = toDelete.begin(); it != toDelete.end(); it++) {
    //         delete *it;
    //     }
    //     toDelete.clear();
    // }
}


#endif /* _TRACE_WORKLOAD_2D_H_ */

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

#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "IndexLog.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/Workload2D.h"
#include "2D/TraceWriter2D.h"
#include "util/Log.h"

using namespace PSimIndex;
using namespace DBUtl;

// TODO: Deep copy pointMap and save file position before starting a tick ONLY
// if compare flag is set. Then we can reset the trace to the beginning of the
// tick in order to get identical behavior (I hope). We can then Get rid of
// all the duplicate code in Experiment2D, and it will be much easier to
// compare ContinuousJoins and regular joins. 

QueryRec* TraceWorkload2D::processQuery(int type,
                                        std::vector<Point2D*>* queryPoints,
                                        double xDelta,
                                        double yDelta,
                                        int k) {
    QueryType qType;

    switch(type) {
    case 0:
        qType = ENUM;
        break;
    case 1:
        qType = AGG;
        break;
    case 2:
        qType = KNN;
        break;
    default:
        //ERROR!!
        break;
    }

    RegionGen2D* rg = new RegionGen2D(xDelta, yDelta);
    return new QueryRec(qType, queryPoints, rg, k);
}

UpdateRec* TraceWorkload2D::processUpdate(int type,
                                          int id,
                                          double x,
                                          double y,
                                          double vx,
                                          double vy,
                                          double data) {

    UpdateType uType;
    Point2D* p;

    switch(type) {
    case 0:
        uType = VEL_UPDATE;
        p = (*pointMap)[id];
        DBUTL_ASSERT((pointMap->count(id) == 1));

        // TODO: Currently only support velocity updates.
//        printf("%lf %lf %lf %lf\n", x, p->x, y, p->y);
//        printf("updates %lf %lf %lf %lf\n", p->vx, vx, p->vy, vy);

//        DBUTL_ASSERT(((x == p->x)));
//        DBUTL_ASSERT(((y == p->y)));
        break;
    case 1:
        uType = AGG_UPDATE;
        p = (*pointMap)[id];
        return new UpdateRec(uType, p, data);
    case 2:
//        if(id == 286) {
//            std::cout << "read pos update " << x << " " << y << std::endl;
//        }

        uType = POS_UPDATE;
        p = (*pointMap)[id];
        pointsPosMove.insert(id);
        break;
    case 3:
        uType = INSERT;
        p = new Point2D(id, x, y, vx, vy, data);

//        printMap();

        // std::cout << "insert ";
        // if(pointMap->count(id) != 0) {
        //     std::cout << "pointMap id = " << id << " count = " 
        //               << pointMap->count(id) << std::endl;
        // }
        // else {
        //     std::cout << std::endl;
        // }



        DBUTL_ASSERT((pointMap->count(id) == 0));
        break;
    case 4:
        uType = DELETE;
        p = (*pointMap)[id];
        pointsPosMove.insert(id);
        break;
    case 5:
        throw std::logic_error("TraceWorkload2D: IMPLICIT_MOVE for \
                                internal use only. .");
        break;
    default:
        throw std::logic_error("TraceWorkload2D: Unknown update type.");
        break;
    }
    return new UpdateRec(uType, p, x, y, vx, vy);
}



void TraceWorkload2D::parseHeaderText() {
    int numRows;
    int read;

    read = fscanf(traceFile, "%d\n", &numRows);
    DBUTL_ASSERT(((read > 0)));

    size = numRows;

    LOG(DEBUG) << "TraceWorkload2D: numPoints = " << numRows << std::endl;

    Point2D* p;



//    for(int i = 0; i < numRows; i++) {
//        p = new Point2D();
//
//        read = fscanf(traceFile, "%d %lf %lf %lf %lf %lf\n",
//                      &(p->id), &(p->x), &(p->y), &(p->vx), &(p->vy), &(p->data));
//        DBUTL_ASSERT(((read > 0)));
//
//        (*pointMap)[p->id] = p;
//        LOG(DEBUG) << "TraceWorkload2D: read point: " << *p << std::endl;
//    }

    for(int i = 0; i < numRows; i++) {
        p = new Point2D();

        read = fscanf(traceFile, "%d %lf %lf\n",
                      &(p->id), &(p->x), &(p->y));
        DBUTL_ASSERT(((read > 0)));

        (*pointMap)[p->id] = p;
        LOG(DEBUG) << "TraceWorkload2D: read point: " << *p << std::endl;
    }

    read = fscanf(traceFile, "%d\n", &numTicks);
    DBUTL_ASSERT(((read > 0)));
    LOG(DEBUG) << "TraceWorkload2D: numTicks = " << numTicks << std::endl;
}

void TraceWorkload2D::parseHeaderBinary() {
    int numRows;
    size_t read;

    read = fread(&numRows, sizeof(int), 1, traceFile);
    DBUTL_ASSERT(((read > 0)));

    size = numRows;
    LOG(DEBUG) << "TraceWorkload2D: numPoints = " << numRows << std::endl;

//    Point2D* ps = new Point2D[numRows];
//    fread(ps, sizeof(Point2D), numRows, traceFile);

    for(int i = 0; i < numRows; i++) {
        Point2D* p = new Point2D();
        read = fread(p, sizeof(Point2D), 1, traceFile);
        DBUTL_ASSERT(((read > 0)));
        (*pointMap)[p->id] = p;
        LOG(DEBUG) << "TraceWorkload2D: read point: " << p << std::endl;
    }

    read = fread(&numTicks, sizeof(int), 1, traceFile);
    DBUTL_ASSERT(((read > 0)));

    LOG(DEBUG) << "TraceWorkload2D: numTicks = " << numTicks << std::endl;
}


void TraceWorkload2D::parseTickBinary() {

    //std::cout << "parsing tick" << std::endl;


    int numQueries;
    size_t read;

    read = fread(&numQueries, sizeof(int), 1, traceFile);
    DBUTL_ASSERT(((read > 0)));

    LOG(DEBUG)<< "TraceWorkload2D: numQueries: " << numQueries << std::endl;

//    queries = new std::vector<QueryRec*>(numQueries, (QueryRec*)0);
    queries = new std::vector<QueryRec*>();
    queries->reserve(numQueries);

    int numIds;
    std::vector<Point2D*>* qPoints;
    int* ids;

    // Read queries from trace.
    for(int i = 0; i < numQueries; i++) {
        // First parse the ids of those points issuing the query.
        read = fread(&numIds, sizeof(int), 1, traceFile);
        DBUTL_ASSERT(((read > 0)));

        //We use 0 as shorthand for all of the points in the index set.
        if(numIds == 0) {
            qPoints = 0;
        }
        else {
            ids = new int[numIds];

            read = fread(ids, sizeof(int), numIds, traceFile);

            qPoints = new std::vector<Point2D*>();
            qPoints->reserve(numIds);

            for(int j = 0; j < numIds; j++) {
                qPoints->push_back((*pointMap)[ids[j]]);
            }
            delete [] ids;
        }
        SerializableQuery q;
        read = fread(&q, sizeof(SerializableQuery), 1, traceFile);
        DBUTL_ASSERT(((read > 0)));

        QueryRec* rec  = processQuery(q.type, qPoints, q.xDelta, q.yDelta, q.k);
        queries->push_back(rec);
        LOG(DEBUG)<< "TraceWorkload2D: Parsed query: " << *rec << std::endl;
    }

    int numUpdates;
    read = fread(&numUpdates, sizeof(int), 1, traceFile);
    DBUTL_ASSERT(((read > 0)));

    LOG(DEBUG)<< "TraceWorkload2D: numUpdates: " << numUpdates << std::endl;

//    updates = new std::vector<UpdateRec*>(numUpdates, (UpdateRec*)0);
    updates->reserve(updates->size() + numUpdates);

    SerializableUpdate* sUpdates = new SerializableUpdate[numUpdates];

    read = fread(sUpdates, sizeof(SerializableUpdate), numUpdates, traceFile);
    DBUTL_ASSERT(((read > 0 || numUpdates == 0)));

// Read updates from trace.
    for(int i = 0; i < numUpdates; i++) {

        UpdateRec* rec = processUpdate(sUpdates[i].type, sUpdates[i].id,
                                       sUpdates[i].x, sUpdates[i].y,
                                       sUpdates[i].vx, sUpdates[i].vy,
                                       sUpdates[i].data);

        updates->push_back(rec);

        LOG(DEBUG)<< "TraceWorkload2D: Parsed update: " << *rec << std::endl;
    }
    delete [] sUpdates;
}

void TraceWorkload2D::parseTickText() {

    int numQueries;
    int read;

//    std::cout << "parse tick pointMap size: " << pointMap->size() << std::endl;
//    std::cout << "parseTickText pointMap" << std::endl;
//    printMap();

    read = fscanf(traceFile, "%d\n", &numQueries);
    DBUTL_ASSERT(((read > 0)));

    LOG(DEBUG)<< "TraceWorkload2D: numQueries: " << numQueries << std::endl;
    queries = new std::vector<QueryRec*>();
    queries->reserve(numQueries);

    int numIds, id;
    int type, k;
    double xDelta, yDelta,x,y;
    Point2D* p;
    std::vector<Point2D*>* qPoints;




//    std::cout << "before queries pointMap size: " << pointMap->size() << std::endl;
//    std::cout << "numQueries: " << numQueries << std::endl;
    // Read queries from trace.
    for(int i = 0; i < numQueries; i++) {
        // First parse the ids of those points issuing the query.
        read = fscanf(traceFile, "%d\n", &numIds);
        DBUTL_ASSERT(((read > 0)));

        //We use 0 as shorthand for all of the points in the index set.
        if(numIds == 0) {
            qPoints = 0;
        }
        else {
            qPoints = new std::vector<Point2D*>(numIds, (Point2D*) 0);
            for(int j = 0; j < numIds; j++) {
 //               read = fscanf(traceFile, "%d %lf %lf\n", &id);
            	p = new Point2D();
            	read = fscanf(traceFile, "%d %lf %lf\n",
            	                      &(p->id), &(p->x), &(p->y));

//                std::cout << "id: " << id << std::endl;
                DBUTL_ASSERT(((read > 0)));
 //               qPoints->at(j) = (*pointMap)[id];
                qPoints->at(j) = p;
            }
        }
        read = fscanf(traceFile, "%d %lf %lf %d\n",
                      &type, &xDelta, &yDelta, &k);
        DBUTL_ASSERT(((read > 0)));

        QueryRec* rec  = processQuery(type, qPoints, xDelta, yDelta, k);
//        std::cout << *rec << std::endl;


        queries->push_back(rec);

        LOG(DEBUG)<< "TraceWorkload2D: Parsed query: " << *rec << std::endl;
    }

    
    // std::cout << "parseTickText pointMap 2" << std::endl;
    // printMap();


//    std::cout << "after queries pointMap size: " << pointMap->size() << std::endl;

    LOG(DEBUG) << "TraceWorkload2D: queries size: " << queries->size()
               << std::endl;
//    int numUpdates;
//
//    read = fscanf(traceFile, "%d\n", &numUpdates);
//    DBUTL_ASSERT(((read > 0)));
//
//    LOG(DEBUG)<< "TraceWorkload2D: numUpdates: " << numUpdates << std::endl;
//    updates->reserve(updates->size() + numUpdates);
//    double x, y, vx, vy, data;

//    std::cout << "before loop pointMap size: " << pointMap->size() << std::endl;

    // Read updates from trace.
//    for(int i = 0; i < numUpdates; i++) {
//        read = fscanf(traceFile, "%d %d %lf %lf %lf %lf %lf\n",
//                      &type, &id, &x, &y, &vx, &vy, &data);
//        DBUTL_ASSERT(((read > 0)));
//
////        std::cout << "loop pointMap size: " << pointMap->size() << std::endl;
//
//        UpdateRec* rec = processUpdate(type, id, x, y, vx, vy, data);
//        updates->push_back(rec);
//        LOG(DEBUG)<< "TraceWorkload2D: Parsed update: " << *rec << std::endl;
//
//    }

//    for(int i = 0; i < numUpdates; i++) {
//        read = fscanf(traceFile, "%d %lf %lf\n", &id, &x, &y);
//        DBUTL_ASSERT(((read > 0)));
//
////        std::cout << "loop pointMap size: " << pointMap->size() << std::endl;
//
//        UpdateRec* rec = processUpdate(type, id, x, y, vx, vy, data);
//        updates->push_back(rec);
//        LOG(DEBUG)<< "TraceWorkload2D: Parsed update: " << *rec << std::endl;
//
//    }
}


void TraceWorkload2D::printMap() {
    //std::tr1::unordered_map<int, Point2D*>::iterator it;
	std::unordered_map<int, Point2D*>::iterator it;
    std::cout << "Printing pointMap size = " << pointMap->size() << std::endl;
    for(it = pointMap->begin(); it != pointMap->end(); it++) {
        std::cout << it->first << " " << *(it->second) << std::endl;
    }
    std::cout << std::endl;
}


std::vector<Point2D*>* TraceWorkload2D::toVector() {
    std::vector<Point2D*>* points = new std::vector<Point2D*>();
    points->reserve(pointMap->size());

    //std::tr1::unordered_map<int, Point2D*>::iterator it;
    std::unordered_map<int, Point2D*>::iterator it;
    for(it = pointMap->begin(); it != pointMap->end(); it++) {
        points->push_back(it->second);
    }
    return points;
}


// void TraceWorkload2D::deepCopyPoints() {
//     std::tr1::unordered_map<int, Point2D*>::iterator it;

//     pointMapCopy = new std::tr1::unordered_map<int, Point2D*>();
    
//     for(it = pointMap->begin(); it != pointMap->end(); it++) {
//         (*pointMapCopy)[it->first] = new Point2D(it->second);
//     }
// }

void TraceWorkload2D::startTick() {
    LOG(DEBUG) << "TraceWorkload2D: Starting tick " << currTick << std::endl;

    //std::tr1::unordered_map<int, Point2D*>::iterator it;
    std::unordered_map<int, Point2D*>::iterator it;


    // std::cout << "Start Tick pointMap: " << std::endl;
    // printMap();



    updates = new std::vector<UpdateRec*>();
    updates->reserve(pointMap->size());


    if(isBinary)
        parseTickBinary();
    else
        parseTickText();


    //printMap();

//    std::vector<UpdateRec*> implicitUpdates;
//
//    //Process implicit move updates before any other updates.
//
//    for(it = pointMap->begin(); it != pointMap->end(); it++) {
//
//        // std::cout << "first: " << it->first << std::endl;
//
//        // std::cout << "second: " << *it->second << std::endl;
//
//        if(pointsPosMove.count(it->second->id) == 0) {
//            implicitUpdates.push_back(new UpdateRec(IMPLICIT_MOVE, it->second));
//
//        }
//
//    }
//
//    updates->insert(updates->begin(), implicitUpdates.begin(), implicitUpdates.end());

    pointsPosMove.clear();


    queryCount = updateCount = 0;
    currTick++;
}

void TraceWorkload2D::endTick() {
    LOG(DEBUG) << "TraceWorkload2D: Ending tick " << currTick << std::endl;

    if(queries != 0)
        clearQueries();
    if(updates != 0)
        clearUpdates();

    // while(toDelete.size() != 0) {
    //     Point2D* p = toDelete.front();
    //     toDelete.pop_front();
    //     delete p;
    // }

    // std::cout << "End Tick pointMap: " << std::endl;
    // printMap();

}




// void TraceWorkload2D::saveState() {
//     DBUTL_ASSERT((pointMapCopy == NULL));
    
// //    fgetpos(traceFile, &filePos);

//     deepCopyPoints();
// }


// void TraceWorkload2D::resetState() {

//     if(queries != 0)
//         clearQueries();
//     if(updates != 0)
//         clearUpdates();

//     //deletePoints();
//     pointsPosMove.clear();


//     std::tr1::unordered_map<int, Point2D*>::iterator it;
//     for(it = pointMap->begin(); it != pointMap->end(); it++) {
//         toDelete.push_back(it->second);
//     }
//     delete pointMap;
//     pointMap = pointMapCopy;
//     pointMapCopy = NULL;

//     // Shallow copy is fine here. 
//     // TODO: Just swap pointers.
//     //pointMap = pointMapCopy




//     //pointMap.insert(pointMapCopy->begin(), pointMapCopy->end());
    

//     //TODO: What happens when called before first tick?
//     currTick--;

//     // Reset the file pointer to the beginning of the last tick. 
//     //fsetpos(traceFile, &filePos);
//     //fgetpos(traceFile, &filePos);
// }


void TraceWorkload2D::deletePoints() {
    //std::tr1::unordered_map<int, Point2D*>::iterator it;
    std::unordered_map<int, Point2D*>::iterator it;

    for(it = pointMap->begin(); it != pointMap->end(); it++) {
        delete it->second;
    }
    
    delete pointMap;

    pointsPosMove.clear();
}


void TraceWorkload2D::reset() {

    if(queries != 0)
        clearQueries();
    if(updates != 0)
        clearUpdates();



    rewind(traceFile);
    currTick = 0;

    deletePoints();

   // pointMap = new std::tr1::unordered_map<int, Point2D*>() ;
    pointMap = new std::unordered_map<int, Point2D*>() ;

    if(isBinary)
        parseHeaderBinary();
    else
        parseHeaderText();

    //fgetpos(traceFile, &filePos);
}




TraceWorkload2D::TraceWorkload2D(const char* filename, 
                                 bool binary/*, 
                                              bool replayable*/) {
    this->filename = filename;
    isBinary = binary;
    traceFile = fopen(filename, (binary ? "rb" : "r"));
    setbuf (traceFile, fileBuffer);
    const char* tmp = strrchr(filename,'/');

    if(tmp == 0)
        sprintf(name, "%s", filename);
    else
        sprintf(name, "%s", ++tmp);

    isBinary = binary;
//    isReplayable = replayable;

    currTick = 0;

    //pointMap = new std::tr1::unordered_map<int, Point2D*>();
    pointMap = new std::unordered_map<int, Point2D*>();

    if(isBinary)
        parseHeaderBinary();
    else
        parseHeaderText();

    //pointMapCopy = NULL;
//    fgetpos(traceFile, &filePos);
}

TraceWorkload2D::~TraceWorkload2D() {

    if(queries != 0)
        clearQueries();
    if(updates != 0)
        clearUpdates();

    deletePoints();
    fclose(traceFile);
}

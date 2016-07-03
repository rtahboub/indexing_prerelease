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

#include "2D/TraceWriter2D.h"
#include "util/AlignedMem.h"
#include "util/Log.h"
#include <cstdio>
#include <cstring>

#include "IndexLog.h"

//for testing
#include <iostream>
using namespace PSimIndex;
using namespace DBUtl;


const int TraceWriter2D::DEFAULT_POINTS;
const int TraceWriter2D::DEFAULT_UPDATES;

void TraceWriter2D::addPoint(int id, double x, double y, 
                             double vx, double vy, double data) {

    // Resize array. 
    if(numPoints >= pointsSize) {
        pointsSize *= 2;
        LOG(DEBUG) << "Resizing points array to " << pointsSize << std::endl;

        Point2D* newPoints = 
            (Point2D*) AlignedMem::allocate(pointsSize*sizeof(Point2D), 64);
        memcpy(newPoints, points, numPoints*sizeof(Point2D));
        AlignedMem::deallocate(points);
        points = newPoints;
    }

    Point2D* p = &points[numPoints++];
    p->id = id;
    p->x = x;
    p->y = y;
    p->vx = vx;
    p->vy = vy;
    p->data = data;

    LOG(DEBUG) << "TraceWriter2D: Added point " << *p << std::endl;

}



void TraceWriter2D::writeHeader(int numTicks) {
    if(isBinary) {
        fwrite(&numPoints, sizeof(int), 1, file);


        fwrite(points, sizeof(Point2D), numPoints, file);        
        fwrite(&numTicks, sizeof(int), 1, file);

    }
    else {
        fprintf(file, "%d\n", numPoints);
        for(int i = 0; i < numPoints; i++) {
            fprintf(file, "%d %lf %lf %lf %lf %lf\n", 
                    points[i].id, points[i].x, points[i].y, 
                    points[i].vx, points[i].vy, points[i].data);
        }

        fprintf(file, "%d\n", numTicks);            
    }
    fflush(file);

    AlignedMem::deallocate(points);

}

void TraceWriter2D::startQueries() {
    currQuery = 0;
}

void TraceWriter2D::addQuerier(int id) {
    while(queriers.size() <= currQuery)
        queriers.push_back(new std::vector<int>());

    queriers[currQuery]->push_back(id);
}

void TraceWriter2D::writeQuery(int type, double xDelta, double yDelta, int k) {

    LOG(DEBUG) << "TraceWriter2D: Adding query (" << type << " " 
               << xDelta << " " << yDelta << " " << k << ")" << std::endl;

    queries.push_back(new SerializableQuery(type, xDelta, yDelta, k));
    currQuery++;
}

void TraceWriter2D::endQueries() {
    int numQueriers;

   // Ugh, this is ugly. 
    while(queriers.size() < queries.size()) {
        queriers.push_back(new std::vector<int>());
    }

    if(isBinary) {
        int size = queries.size();
        fwrite(&size, sizeof(int), 1, file);

        for(int i = 0; i < size; i++)  {
//            std::cout << "queriers size: " << queriers.size() << std::endl;
//            std::cout << "queriers[i] size: " << queriers[i]->size() << std::endl;
//            std::cout << "queries size: " << queries.size() << std::endl;
//            std::cout << "Accessing : " << i << std::endl;

            numQueriers = queriers[i]->size();
            fwrite(&numQueriers, sizeof(int), 1, file);
            if(queriers[i]->size() > 0)
                fwrite(&queriers[i]->at(0), sizeof(int), 
                       queriers[i]->size(), file);
            fwrite(queries[i], sizeof(SerializableQuery), 1, file);
        }
    }
    else {
        fprintf(file, "%d\n", (int) queries.size());        

        for(int i = 0; i < queries.size(); i++) {
            numQueriers = queriers[i]->size();

            fprintf(file, "%d\n", numQueriers);            
            for(int j = 0; j < numQueriers; j++) {
                fprintf(file, "%d\n", queriers[i]->at(j));
            }
            

            fprintf(file, "%d %lf %lf %d\n", 
                    queries[i]->type, queries[i]->xDelta, 
                    queries[i]->yDelta, queries[i]->k);
        }
    }

    for(int i = 0; i < queries.size(); i++) {
        delete queries[i];
        delete queriers[i];
    }
    queries.clear();
    queriers.clear();

}

void TraceWriter2D::startUpdates() {
}

void TraceWriter2D::writeUpdate(int type, int id, double x, double y, 
                                double vx, double vy, double data) {
    

    // Resize array. 
    if(numUpdates >= updatesSize) {
        updatesSize *= 2;
        //std::cout << "updatesSize " << updatesSize << std::endl;
        
        int newSize = updatesSize * sizeof(SerializableUpdate);

        SerializableUpdate* newUpdates = 
            (SerializableUpdate*) AlignedMem::allocate(newSize, 64);
        memcpy(newUpdates, updates, numUpdates*sizeof(SerializableUpdate));
        AlignedMem::deallocate(updates);
        updates = newUpdates;
    }


    SerializableUpdate* u = &updates[numUpdates++];
    u->type = type;
    u->id = id;
    u->x = x;
    u->y = y;
    u->vx = vx;
    u->vy = vy;
    u->data = data;

    LOG(DEBUG) << "TraceWriter2D: Adding Update (" 
               << u->type << " "
               << u->id << " "
               << u->x << " "
               << u->y << " "
               << u->vx << " "
               << u->vy << " "
               << u->data << ")" << std::endl;
}

void TraceWriter2D::endUpdates() {

    if(isBinary) {
        fwrite(&numUpdates, sizeof(int), 1, file);
        fwrite(updates, sizeof(SerializableUpdate), numUpdates, file);        
    }
    else {
        fprintf(file, "%d\n", numUpdates);
        for(int i = 0; i < numUpdates; i++) {
            fprintf(file, "%d %d %lf %lf %lf %lf %lf\n", 
                    updates[i].type, updates[i].id,
                    updates[i].x, updates[i].y,
                    updates[i].vx, updates[i].vy, updates[i].data);
        }
    }
}

void TraceWriter2D::endTick() {
    numUpdates = 0;
}


void TraceWriter2D::binaryToText(const char* binaryFile, const char* textFile) {
    FILE* binary = fopen(binaryFile, "rb");
    FILE* text = fopen(textFile, "w");
    size_t read;

    //translate header
    int numRows, numTicks;
    read = fread(&numRows, sizeof(int), 1, binary);
    DBUTL_ASSERT(((read > 0)));

    fprintf(text, "%d\n", numRows);


    Point2D* points = new Point2D[numRows];
    read = fread(points, sizeof(Point2D), numRows, binary);
    DBUTL_ASSERT(((read > 0)));
    for(int i = 0; i < numRows; i++) {
        fprintf(text, "%d %lf %lf %lf %lf %lf\n", 
                points[i].id, points[i].x, points[i].y, 
                points[i].vx, points[i].vy, points[i].data);
    }
    
    read = fread(&numTicks, sizeof(int), 1, binary);
    DBUTL_ASSERT(((read > 0)));
    fprintf(text, "%d\n", numTicks);

    for(int i = 0; i < numTicks; i++) {

        // Translate queries
        int numQueries, numIds, id;

        read = fread(&numQueries, sizeof(int), 1, binary);        
        DBUTL_ASSERT(((read > 0)));

        fprintf(text, "%d\n", numQueries);

        for(int j = 0; j < numQueries; j++) {

            read = fread(&numIds, sizeof(int), 1, binary);
            DBUTL_ASSERT(((read > 0)));

            fprintf(text, "%d\n", numIds);

            if(numIds > 0) {

                int* ids = new int[numIds];

                read = fread(ids, sizeof(int), numIds, binary);
                DBUTL_ASSERT(((read > 0)));

                for(int k = 0; k < numIds; k++) {
                    fprintf(text, "%d\n", ids[k]);
                }
            }

            SerializableQuery q;
            
            read = fread(&q, sizeof(SerializableQuery), 1, binary);
            DBUTL_ASSERT(((read > 0)));

            fprintf(text, "%d %lf %lf %d\n", 
                   q.type, q.xDelta, q.yDelta, q.k);

        }

        //Translate Updates

        int numUpdates;
        
        read = fread(&numUpdates, sizeof(int), 1, binary);
        DBUTL_ASSERT(((read > 0)));

        fprintf(text, "%d\n", numUpdates);

        SerializableUpdate u;

        for(int j = 0; j < numUpdates; j++) {
            read = fread(&u, sizeof(SerializableUpdate), 1, binary);
            DBUTL_ASSERT(((read > 0)));

            fprintf(text, "%d %d %lf %lf %lf %lf %lf\n", 
                   u.type, u.id, u.x, u.y, u.vx, u.vy, u.data);

        }

    }

    fclose(binary);
    fclose(text);
}

// Not super worried about performance here, so do the translation one value
// at a time. 
void TraceWriter2D::textToBinary(const char* textFile, const char* binaryFile) {

    FILE* text = fopen(textFile, "r");
    FILE* binary = fopen(binaryFile, "wb");
    int read; 

    // Translate Header
    int numRows, numTicks;
    read = fscanf(text, "%d\n", &numRows);
    DBUTL_ASSERT(((read > 0)));

    fwrite(&numRows, sizeof(int), 1, binary);

    Point2D p;
    for(int i = 0; i < numRows; i++) {

        read = fscanf(text, "%d %lf %lf %lf %lf %lf\n",
                      &(p.id), &(p.x), &(p.y), &(p.vx), &(p.vy), &(p.data));
        DBUTL_ASSERT(((read > 0)));
        fwrite(&p, sizeof(Point2D), 1, binary);

    }

    read = fscanf(text, "%d\n", &numTicks);
    DBUTL_ASSERT(((read > 0)));    
    fwrite(&numTicks, sizeof(int), 1, binary);



    for(int i = 0; i < numTicks; i++) {

        // Translate queries
        
        int numQueries, numIds, id;

        
        read = fscanf(text, "%d\n", &numQueries);
        DBUTL_ASSERT(((read > 0)));
        fwrite(&numQueries, sizeof(int), 1, binary);        

        
        for(int j = 0; j < numQueries; j++) {

            read = fscanf(text, "%d\n", &numIds);
            DBUTL_ASSERT(((read > 0)));
            fwrite(&numIds, sizeof(int), 1, binary);

            for(int k = 0; k < numIds; k++) {
                read = fscanf(text, "%d\n", &id);  
                DBUTL_ASSERT(((read > 0)));
                fwrite(&id, sizeof(int), 1, binary);            
            }

            SerializableQuery q;
            
            read = fscanf(text, "%d %lf %lf %d\n", 
                          &q.type, &q.xDelta, &q.yDelta, &q.k);
            DBUTL_ASSERT(((read > 0)));

            fwrite(&q, sizeof(SerializableQuery), 1, binary);
        }

        // Translate updates
        int numUpdates;
        
        read = fscanf(text, "%d\n", &numUpdates);
        DBUTL_ASSERT(((read > 0)));
        fwrite(&numUpdates, sizeof(int), 1, binary);

        SerializableUpdate u;

        for(int j = 0; j < numUpdates; j++) {
            read = fscanf(text, "%d %d %lf %lf %lf %lf %lf\n", 
                          &u.type, &u.id, &u.x, &u.y, &u.vx, &u.vy, &u.data);
            DBUTL_ASSERT(((read > 0)));

            fwrite(&u, sizeof(SerializableUpdate), 1, binary);            
        }
    }

    fclose(text);
    fclose(binary);
}






TraceWriter2D::TraceWriter2D(const char* filename, bool binary) {
//    file = fopen(filename, (binary ? "wb" : "w"));
    file = fopen(filename, "wb");
    if(file == NULL) {
        std::cout << "filename = " << filename << " file is NULL!" << std::endl;
    }

    isBinary = binary;

    pointsSize = DEFAULT_POINTS;
    numPoints = 0;
    points = (Point2D*) AlignedMem::allocate(pointsSize*sizeof(Point2D), 64);


    updatesSize = DEFAULT_UPDATES;
    numUpdates = 0;
    int uSize = updatesSize*sizeof(SerializableUpdate);

    updates = (SerializableUpdate*) AlignedMem::allocate(uSize, 64);
}

TraceWriter2D::~TraceWriter2D() {
    fclose(file);

    AlignedMem::deallocate(updates);
}

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

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cmath>

#include "2D/DataGenerator.h"
#include "2D/data_generator/UniformGenerator.h"
#include "util/RNG.h"
#include "util/Log.h"
#include "IndexLog.h"

//testing
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

using namespace PSimIndex;
using namespace DBUtl;

Point2D* UniformGenerator::createPoint() {
    double x = rng->rand() * (maxGenX - minGenX) + minGenX;
    double y = rng->rand() * (maxGenY - minGenY) + minGenY;

    int speedIndex = rng->uniformInt(0, numSpeeds);

    double mySpeed = speed[speedIndex];
    double theta = rng->rand() * (2.0 * M_PI);

    double vx = mySpeed * cos(theta);
    double vy = mySpeed * sin(theta);

    double data = rng->rand();

    if(intTrace)
        return new Point2D((int)x, (int)y, (int)vx, (int)vy, (int)data);
    else
        return new Point2D(x, y, vx, vy, data);
}

void UniformGenerator::genPoints() {
    Point2D* p;
    for(int i = 0; i < numPoints; i++) {
        p = createPoint();
        LOG(DEBUG) << "UniformGenerator: Generating Point " << *p << std::endl;

        writer->addPoint(p->id, p->x, p->y, p->vx, p->vy, p->data);

        pointMap[p->id] = p;
    }
}

void UniformGenerator::writeIntUpdate(int type, int id, double x, double y, 
                                      double vx, double vy, double data) {
    if(intTrace) {
        writer->writeUpdate(type, id, (int) x, (int) y, (int) vx, (int) vy, (int) data);
    }
    else {
        writer->writeUpdate(type, id, x, y, vx, vy, data);
    }
}

//void UniformGenerator::genTick(std::tr1::unordered_set<int>* queriers) {
void UniformGenerator::genTick(std::unordered_set<int>* queriers) {
    // std::cout << "allDeletes: " << allDeletes << std::endl;
    // std::cout << "allInserts: " << allInserts << std::endl;


    Point2D* p;
    //std::vector<Point2D*>::iterator it;
//    std::tr1::unordered_map<int, Point2D*>::iterator it;
//    std::tr1::unordered_set<int, Point2D*>::iterator qIt;
    	std::unordered_map<int, Point2D*>::iterator it;
       std::unordered_set<int, Point2D*>::iterator qIt;

    // Generate queries
    writer->startQueries();

    //TODO: What if I don't want any queriers?
    if(probQuery < 1.0) {
        //continuous queries
        if(contQueries/*queriers != NULL*/) {
            for(qIt = queriers->begin(); qIt != queriers->end(); qIt++) {
                writer->addQuerier(*qIt);  
            }
        }
        else {
            for(it = pointMap.begin(); it != pointMap.end(); it++) {
                if(rng->rand() < probQuery)
                    writer->addQuerier(it->first);
            }
        }
    }

    //Just range queries for now.
    double delta = rng->rand()*(qMax - qMin) + qMin;

    if(intTrace)
        writer->writeQuery(0, (int)delta,(int)delta, 0);
    else
        writer->writeQuery(0, delta, delta, 0);

    writer->endQueries();
     
    // Generate updates
    writer->startUpdates();

    double oldVx, oldVy;
    bool updated;


   // std::tr1::unordered_set<int> deleteIds;
    std::unordered_set<int> deleteIds;
    std::vector<Point2D*> insertPoints;

    for(it = pointMap.begin(); it != pointMap.end(); it++) {
        updated = false;
        p = it->second;

        double newX = p->x + p->vx;
        double newY = p->y + p->vy;

        bool posUp = false;

        while(newX >= maxX) {
            newX = newX - maxX;
            posUp = true;
        }
        while(newX < 0) {
            newX = newX + maxX;
            posUp = true;
        }
        while(newY >= maxY) {
            newY = newY - maxY; 
            posUp = true;
        }
        while(newY < 0) {
            newY = newY + maxY;
            posUp = true;
        }

        if(posUp) {
            DBUTL_ASSERT((newX < maxX));
            DBUTL_ASSERT((newY < maxY));


            if(allDeletes) {
                writeIntUpdate(4, p->id, p->x, p->y, p->vx, p->vy, p->data);
                deleteIds.insert(p->id);

            }
            else if(!allInserts){
                writeIntUpdate(2, p->id, (int) newX, (int) newY, p->vx, p->vy, p->data);
            }
            



            //TMP: Delete point instead of moving it. 



        }

        // Update velocity. 
//        if(gsl_ran_flat(r, 0, 1) < probUpdate) {
        if(rng->rand() < probUpdate) {
            
            if(allDeletes) {
                //Avoid deleting an element twice. 
                if(deleteIds.count(p->id) == 0) {
                    writeIntUpdate(4, p->id, p->x, p->y, p->vx, p->vy, p->data);
                }
                deleteIds.insert(p->id);
            }
            else if(allInserts) {
                Point2D* newP = createPoint();

                writeIntUpdate(3, newP->id, newP->x, newP->y, 
                               newP->vx, newP->vy, newP->data);

                insertPoints.push_back(newP);
            }
            else {
                updated = true;

                //double newVx, newVy;

                double mySpeed = speed[rng->uniformInt(0, numSpeeds)];
                double theta = rng->rand() * (2.0 * M_PI);
                //double theta = gsl_ran_flat(r, 0, 1) * (2.0 * M_PI);

                p->vx = mySpeed * cos(theta);
                p->vy = mySpeed * sin(theta);
                if(intTrace) {
                    p->vx = (int)p->vx;
                    p->vy = (int)p->vy;
                }

                writeIntUpdate(0, p->id, p->x, p->y, p->vx, p->vy, p->data);

            }
        }

        p->x = newX;
        p->y = newY;

        DBUTL_ASSERT((p->x < maxX));
        DBUTL_ASSERT((p->y < maxX));

    }


    //std::tr1::unordered_set<int>::iterator dIt;
    std::unordered_set<int>::iterator dIt;
    for(dIt = deleteIds.begin(); dIt != deleteIds.end(); dIt++) {
        pointMap.erase(*dIt);

        if(contQueries) {
            queriers->erase(*dIt);
        }
    }
    deleteIds.clear();


    std::vector<Point2D*>::iterator iIt;
    for(iIt = insertPoints.begin(); iIt != insertPoints.end(); iIt++) {
        pointMap[(*iIt)->id] = *iIt;
    }
    insertPoints.clear();

    // std::cout << "Points after delete: " <<std::endl;
    // for(int i = 0; i < points.size(); i++) {
    //     std::cout << *points[i] << std::endl;
    // }
    // std::cout << std::endl;


    writer->endUpdates();
    writer->endTick();
}

void UniformGenerator::setParam(const char* param) {

//    LOG(DEBUG) << "UniformGenerator: Setting param " << param << std::endl;

    const char* v;

    if((v = testParam(param, "--int=")) != 0) {

        if(strcmp(v, "true") == 0 || strcmp(v, "True") == 0) {
            setInt(true);
        }
        else if(strcmp(v, "false") == 0 || strcmp(v, "False") == 0) {
            setInt(false);
        }
        else {
            throw std::logic_error("--int param takes true or false.");
        }
    }
    else if((v = testParam(param, "--contQueries=")) != 0) {

        if(strcmp(v, "true") == 0 || strcmp(v, "True") == 0) {
            setCont(true);
        }
        else if(strcmp(v, "false") == 0 || strcmp(v, "False") == 0) {
            setCont(false);
        }
        else {
            throw std::logic_error("--contQueries param takes true or false.");
        }
    }
    else if((v = testParam(param, "--allDeletes=")) != 0) {

        if(strcmp(v, "true") == 0 || strcmp(v, "True") == 0) {

            if(allInserts)
                throw std::logic_error("Error: Can't set both --allDeletes and --allInserts.");


            setAllDeletes(true);
        }
        else if(strcmp(v, "false") == 0 || strcmp(v, "False") == 0) {
            setAllDeletes(false);
        }
        else {
            throw std::logic_error("--allDeletes param takes true or false.");
        }
    }
    else if((v = testParam(param, "--allInserts=")) != 0) {

        if(strcmp(v, "true") == 0 || strcmp(v, "True") == 0) {
            if(allDeletes)
                throw std::logic_error("Error: Can't set both --allDeletes and --allInserts.");
            setAllInserts(true);
        }
        else if(strcmp(v, "false") == 0 || strcmp(v, "False") == 0) {
            setAllInserts(false);
        }
        else {
            throw std::logic_error("--allInserts param takes true or false.");
        }
    }
    else if((v = testParam(param, "--maxX=")) != 0) {
        setMaxX(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxY=")) != 0) {
        setMaxY(strtod(v, 0));
    }
    else if((v = testParam(param, "--minGenX=")) != 0) {
        setMinGenX(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxGenX=")) != 0) {
        setMaxGenX(strtod(v, 0));
    }
    else if((v = testParam(param, "--minGenY=")) != 0) {
        setMinGenY(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxGenY=")) != 0) {
        setMaxGenY(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxSpeed=")) != 0) {
        setMaxSpeed(strtod(v, 0));
    }
    else if((v = testParam(param, "--numSpeeds=")) != 0) {
//        std::cout << "setting numSpeeds " << strtol(v, 0, 0) << std::endl;
        setNumSpeeds(strtol(v, 0, 0));
    }
    else if((v = testParam(param, "--qMin=")) != 0) {
        setQMin(strtod(v, 0));
    }
    else if((v = testParam(param, "--qMax=")) != 0) {
        setQMax(strtod(v, 0));
    }
    else if((v = testParam(param, "--probQuery=")) != 0) {
        double probQuery = strtod(v, 0);
        setProbQuery(probQuery);
        if(probQuery > 1) {
            throw std::logic_error("probQuery must be a probability");
        }
    }
    else if((v = testParam(param, "--probUpdate=")) != 0) {
        double probUpdate = strtod(v, 0);
        setProbUpdate(probUpdate);
        if(probUpdate > 1) {
            throw std::logic_error("probUpdate must be a probability");
        }
    }
    else {
        DataGenerator::setParam(param);
    }
}


void UniformGenerator::genTrace() {
    //sowell tmp remove;
    //rng = &RNG::get();

   // std::tr1::unordered_map<int, Point2D*>::iterator it;
	std::unordered_map<int, Point2D*>::iterator it;


    openTrace();

    // If the region in which to generate the points is not set, then it
    // should default to the space size.
    if(minGenX == -1) {
        minGenX = 0;
    }
    if(maxGenX == -1) {
        maxGenX = maxX;
    }
    if(minGenY == -1) {
        minGenY = 0;
    }
    if(maxGenY == -1) {
        maxGenY = maxY;
    }



    speed = new double[numSpeeds];

    for(int i=0;i<numSpeeds;i++) {
        speed[i] = maxSpeed * (i+1) / numSpeeds;
    }

//    std::cout << "numSpeeds: " << numSpeeds << std::endl;

    genPoints();
    writer->writeHeader(numTicks);

    
    //std::vector<int>* queriers = NULL;
    //std::tr1::unordered_set<int>* queriers = NULL;
    std::unordered_set<int>* queriers = NULL;

    //Pick fixed set of points that will query for the entire simulation.
    if(contQueries) {
       // queriers = new std::tr1::unordered_set<int>();
    	queriers = new std::unordered_set<int>();
        for(it = pointMap.begin(); it != pointMap.end(); it++) {
            if(rng->rand() < probQuery)
                queriers->insert(it->first);
        }
    }
    // else {
    //     queriers = NULL;
    // }

    for(int i = 0; i < numTicks; i++) {

        genTick(queriers);


        // for(it = points.begin(); it != points.end(); it++) {
        //     Point2D* p = *it;
        //     if(p->x < 0 || p->y < 0) {
        //         std::cout << "ERROR: less than 0 " << *p << std::endl;
        //     }
        // }

    }

    delete queriers;

    closeTrace();
}

UniformGenerator::UniformGenerator() {
    this->intTrace = false;
    this->contQueries = false;
    this->allDeletes = false;
    this->allInserts = false;
    this->maxX = 100000;
    this->maxY = 100000;


    this->minGenX = -1;
    this->maxGenX = -1;
    this->minGenY = -1;
    this->maxGenY = -1;


    this->maxSpeed = 100;
    this->numSpeeds = 10;
    this->qMin = 1000;
    this->qMax = 1000;
    this->probQuery = 1.0;
    this->probUpdate = 1.0;
}

UniformGenerator::~UniformGenerator() {
//    delete writer;
    delete [] speed;

    //std::tr1::unordered_map<int, Point2D*>::iterator it;
    std::unordered_map<int, Point2D*>::iterator it;
    for(it = pointMap.begin(); it != pointMap.end(); it++) {
        delete it->second;
    }
    pointMap.clear();
}

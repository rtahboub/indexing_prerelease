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
#include <ctime>
#include <cmath>

#include "2D/data_generator/GaussianGenerator.h"
#include "2D/DataGenerator.h"

#include "util/Log.h"
#include "IndexLog.h"

//Use gsl for gaussian inverse cdf.
//#include "DBUtl/include/RNG.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>


using namespace PSimIndex;


bool GaussianGenerator::compareUpdates(GaussUpdateRec* u1, GaussUpdateRec* u2) {
    return ((u1->tick < u2->tick) || (u1->tick == u2->tick && u1->id < u2->id));
}


bool GaussianGenerator::testCompareUpdates(GaussUpdateRec* u1, GaussUpdateRec* u2) {
    return ((u1->id < u2->id) || (u1->id == u2->id && u1->tick < u2->tick));
}




double GaussianGenerator::genSafePos(Point2D* p, int& spotNum) {


    bool spotSet = true;

    //Pick random hotspot. 
    if(spotNum == -1) {
        //spotSet = false;
        spotNum = gsl_rng_uniform_int(r, numHotspots);
    }

    double distance;
    double theta;    
    int count; 

    // Outer loop tries to avoid getting stuck with a hotspot on the edge of
    // the space. However, if the spotNum is already set, there's not much we
    // can do. 
//    do {
//        if(!spotSet) {
//            spotNum = gsl_rng_uniform_int(r, numHotspots);
//        }

        count = 0;
        do {


            theta = gsl_ran_flat(r, 0, (2.0 * M_PI));

            distance = fabs(gsl_ran_gaussian(r, 0.5)) * radius;

            // std::cout << "radius: " << radius 
            //           << " theta: " << (180.0/M_PI)*theta 
            //           << " distance: " << distance << std::endl;

            // std::cout << "hotspots->x: " << hotspots[spotNum]->x
            //           << " hotspots->y: " << hotspots[spotNum]->y << std::endl;

            p->x = hotspots[spotNum]->x + distance * cos(theta);
            p->y = hotspots[spotNum]->y + distance * sin(theta);
            

            // if(p->x < 0 || p->x >= space || p->y < 0 || p->y >= space) {
            //     std::cout << "p->x = " << p->x << " p->y = " << p->y << std::endl;
            //     std::cout << "count: " << count << std::endl;

            // }
            count++;
            

        } while(//count < 50
            /*&&*/ (p->x < 0 || p->x >= space || p->y < 0 || p->y >= space));

//        std::cout << "count: " << count << std::endl;
//    } while(count >= 50);


    return distance;
}

/**
 * Currently all points are equally likely to query. We don't take into
 * account the ring. 
 */
void GaussianGenerator::genTickQueries(int tick) {


    writer->startQueries();

    //Only issue queries once all of the points have been inserted.
    if(tick >= insertTicks) {

        // Agent ids are dense. 
        if(probQuery < 1.0) {
            for(int i = 0; i < numPoints; i++) {
                if(gsl_ran_flat(r, 0, 1) < probQuery)
                    writer->addQuerier(i);
            }
        }

        //Just range queries for now.
        double delta = gsl_ran_flat(r, 0, 1)*(qMax - qMin) + qMin;

        writer->writeQuery(0, delta, delta, 0);

    }

    writer->endQueries();
}


void GaussianGenerator::genUpdates() {

    Point2D* nextpos = new Point2D();
    Point2D::resetID();

    Point2D* p;
    int spotNum;
    double distance, nextDist;
    int nextTup;

    GaussUpdateRec* gur; 

    for(int i = 0; i < numPoints; i++) {
//        std::cout << "Processing point " << i << ": ";

        p = new Point2D();

        spotNum = -1;

        // Generate the initial position of p. The variable distance records
        // the distance from the hotspot that p is near. 
        distance = genSafePos(p, spotNum);



        int k = 0;

        while(k < numSpeeds - 1 && distance > ring[k+1] * radius) {
            k++;
        }

        double v = speed[k];
        int t = tup[k];

        // Generate the next position. 

//        std::cout << "before gen next position" << std::endl;

        do {
            distance = genSafePos(nextpos, spotNum);
            nextDist = sqrt(pow(nextpos->x - p->x, 2.0) 
                            + pow(nextpos->y - p->y, 2.0));

            // v*t = max distance travelled before next update
        } /*while(nextDist < v * t);*/ while(nextDist > v * t);
//        std::cout << "after genSafePos " << nextDist << " v*t: " << (v*t) << std::endl;


        p->vx = (nextpos->x - p->x) / t;
        p->vy = (nextpos->y - p->y) / t;
        
        // if(p->id == 3) {
        //     std::cout << "Initial Pos: " << *p << " ::: " << t << std::endl;
        //     std::cout << "next pos: " << *nextpos << std::endl;
        // } 
        
        // Add a copy of p to the list of points. We will continue to update
        // p, so we need to record the initial position.
        //points.push_back(new Point2D(p));
        
        // Don't need to add update, because initial point has correct
        // velocity. 

        int iTime = p->id % insertTicks;
        gur = new GaussUpdateRec(iTime, p->id, p->x, p->y, p->vx, p->vy, 3);
        updates.push_back(gur);


            


        nextTup = iTime + t;

        // This differs from Chen et al. Don't worry about rounds -- just
        // specify the maximum number of ticks.
        while(nextTup < numTicks) {

            p->x = nextpos->x;
            p->y = nextpos->y;

            DBUTL_ASSERT((p->x >= 0 && p->x < space && p->y >= 0 && p->y < space));

            k = 0; 

            while(k < numSpeeds - 1 && distance > ring[k+1] * radius) {
                k++;
            }

            v = speed[k];
            t = tup[k];

            do {
                distance = genSafePos(nextpos, spotNum);
                nextDist = sqrt(pow(nextpos->x - p->x, 2.0) 
                                + pow(nextpos->y - p->y, 2.0));
                // v*t = max distance travelled before next update
            } /*while(nextDist < v * t); */ while(nextDist > v*t);

            // if(p->id == 3)
            //     std::cout << "next pos: " << *nextpos << std::endl;

            p->vx = (nextpos->x - p->x) / t;
            p->vy = (nextpos->y - p->y) / t;


            gur = new GaussUpdateRec(nextTup, p->id, p->x, p->y, p->vx, p->vy);

            // if(p->id == 3) {
            //     std::cout << "update: ";
            //     gur->print();
            // }
            updates.push_back(gur);
            
            //nextTup
            nextTup = nextTup + t;

            //std::cout  << "nextTup: " << nextTup << std::endl;
        }
        delete p;
    }

    // Sort updates by time and id.
    std::sort(updates.begin(), updates.end(), compareUpdates);

    //std::sort(updates.begin(), updates.end(), testCompareUpdates);
    // for(int i = 0; i < updates.size(); i++) {
    //     updates[i]->print();
    // }
}



void GaussianGenerator::genTrace() {
    openTrace();


    //std::cout << "opened trace" << std::endl;


    // Generate hotspots. 
    hotspots.reserve(numHotspots);

    for(int i = 0; i < numHotspots; i++) {
        double x = gsl_ran_flat(r, 0, 1) * space;
        double y = gsl_ran_flat(r, 0, 1) * space;
        hotspots[i] = new Point2D(x, y, 0.0);
    }

    //std::cout << "initialized hotspots" << std::endl;


    //Initialize speed.
    speed.reserve(numSpeeds);
    for(int i=0;i<numSpeeds;i++) {
        speed[i] = maxSpeed * (i+1) / numSpeeds;
    }

    //std::cout << "initialized speeds" << std::endl;


    //Initialize update times. 
    tup.reserve(numSpeeds);
    for(int i = 0; i < numSpeeds; i++) {

        // TODO: round??  Okay for now with maxtup = 120, numSpeeds = 10. Need
        // to revisit later. 
        tup[i] =  maxTup * ((numSpeeds - i)) / numSpeeds;
    }

    
    //std::cout << "initialized update times" << std::endl;

    // Initialize rings.
    ring.reserve(numSpeeds);
    for(int i = 1; i <= numSpeeds; i++) {
        ring[numSpeeds - i] 
            = fabs(gsl_cdf_gaussian_Pinv(0.5 / numSpeeds * i, 0.5));
    }





    // Generate radius of gaussian around hotspot.
    radius = space / sqrt(numHotspots * M_PI);

    //test
    // hotspots[numHotspots - 1] = new Point2D(99.99, 99.99, 0.0);
    // Point2D* p = new Point2D();
    // int val = numHotspots - 1;

    // for(int i = 0; i < 10; i++) {
    //     genSafePos(p, val);
    // }

    //std::cout << "initialized rings" << std::endl;

    genUpdates();

//    std::cout << "points.size() = " << points.size() << std::endl;
    for(int i = 0; i < points.size(); i++) {
        Point2D* p = points[i];
        writer->addPoint(p->id, p->x, p->y, p->vx, p->vy, p->data);            
    }

    //std::cout << "numTicks = " << numTicks << std::endl;
    writer->writeHeader(numTicks);

//    std::cout << "updates.size() = " << updates.size() << std::endl;

    int uPtr = 0;

//    std::cout << "numTicks: " << numTicks << " insertTicks: " << insertTicks << std::endl;

    for(int i = 0; i < numTicks; i++) {
        //std::cout << "starting tick " << i << std::endl;
        genTickQueries(i);
        writer->startUpdates();

        while(uPtr < updates.size() && updates[uPtr]->tick == i) {
            GaussUpdateRec* uRec = updates[uPtr++];

            // if(uRec->id == 3)
            //     uRec->print();

            writer->writeUpdate(uRec->type, uRec->id, uRec->x, uRec->y,
                                uRec->vx, uRec->vy, 0.0);
        }

        writer->endUpdates();
        writer->endTick();
        //std::cout << "finished tick " << i << std::endl;
    }
    
    closeTrace();
}


void GaussianGenerator::setParam(const char* param) {

    const char* v;

    if((v = testParam(param, "--space=")) != 0) {
        setSpace(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxSpeed=")) != 0) {
        setMaxSpeed(strtod(v, 0));
    }
    else if((v = testParam(param, "--numSpeeds=")) != 0) {
        setNumSpeeds(strtol(v, 0, 0));
    }
    else if((v = testParam(param, "--numHotspots=")) != 0) {
        setNumHotspots(strtol(v, 0, 0));
    }
    else if((v = testParam(param, "--insertTicks=")) != 0) {
        setInsertTicks(strtol(v, 0, 0));
    }
    else if((v = testParam(param, "--maxTup=")) != 0) {
        setMaxTup(strtol(v, 0, 0));
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
    else {
        DataGenerator::setParam(param);
    }
}

GaussianGenerator::GaussianGenerator() {
    const gsl_rng_type * T;
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
//    gsl_rng_set(r, time(NULL));
    gsl_rng_set(r, 54873518);

    //Defaults for testing

    numTicks = 30;
    numPoints = 10;
    
    insertTicks = 10;

    numHotspots = 10;
    numSpeeds = 2;
    maxTup = 6;

    space = 100.0;
    maxSpeed = 10.0;
    
    qMin = 10.0;
    qMax = 10.0;
    probQuery = 1.0;
}


GaussianGenerator::~GaussianGenerator() {
}



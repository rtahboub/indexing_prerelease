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
#include <cstdlib>
#include <stdexcept>
#include <iostream>

#include "2D/data_generator/NetworkGenerator.h"

using namespace PSimIndex;

void NetworkGenerator::genPoints() {
    for(int i = 0; i < numPoints; i++) {
        Edge* edge;

        do {

            edge = network->getRandomEdge();
            //std::cout << "in loop" << std::endl;
       } while (edge->length == 0);

        double offset = rng->rand();

        double x = offset * (edge->e->x - edge->s->x) + edge->s->x;
        double y = offset * (edge->e->y - edge->s->y) + edge->s->y;

        Node* dest;
        if(rng->rand() < 0.5) {
            dest = edge->e;
            toEnd.push_back(true);
        }
        else {
            dest = edge->s;
            toEnd.push_back(false);
        }

        double v = speed[rng->uniformInt(0, numSpeeds)];
        double theta = atan2(dest->y - y, dest->x - x);

        double vx = v * cos(theta);
        double vy = v * sin(theta);

        double data = rng->rand();

        Point2D* p = new Point2D(x, y, vx, vy, data);

        writer->addPoint(p->id, p->x, p->y, p->vx, p->vy, p->data);
        points.push_back(p);
        currEdge.push_back(edge);
    }
}


void NetworkGenerator::genTick(int tickNum) {

    // Generate queries. We do this the same way we do for uniform
    // traces. Points query independently with some probability. 
    writer->startQueries();

    std::vector<Point2D*>::iterator it;

    if(probQuery < 1.0) {
        for(it = points.begin(); it != points.end(); it++) {
            if(rng->rand() < probQuery)
                writer->addQuerier((*it)->id);
        }
    }

    //Just range queries for now.
    double delta = rng->rand()*(qMax - qMin) + qMin;
    
    writer->writeQuery(0, delta, delta, 0);
    writer->endQueries();
    
    writer->startUpdates();


    
    Point2D* p;
    Edge *edge, *nextEdge;
    Node *start, *dest;
    bool te;
    double targetDist, tickDist;
    double newX, newY, newVx, newVy;


    for(int i = 0; i < points.size(); i++) {
        p = points[i];
        edge = currEdge[i];
        dest = ((toEnd[i]) ? edge->e : edge->s);

        bool fromStart = toEnd[i]; 

        targetDist = dist(p->x, p->y, dest->x, dest->y);
        tickDist = sqrt(p->vx*p->vx + p->vy*p->vy);


        if(p->id == 3)
            std::cout << tickNum << ": " << *p << std::endl;

        // if(p->id == 3) {
        //     std::cout << "dest:" << *dest << " edge length: " << edge->length<< std::endl;
        //     std::cout << "fromStart:" << fromStart << std::endl;
        //     std::cout << "tickDist: " << tickDist << std::endl;
        //     std::cout << "targetDist: " << targetDist << std::endl;
        // }


        // Common case. Point will remain on the road network for the next
        // tick. Update the position, but don't issue an update. 
        if(tickDist < targetDist) {
            // if(p->id == 3)
            //     std::cout << "in if" << std::endl;
            
            newX = p->x + p->vx;
            newY = p->y + p->vy;
            newVx = p->vx;
            newVy = p->vy;
        }
        // If the point will move farther than the distance to the target (the
        // end of the road), then we need to switch to a new road. This is in
        // a while loop rather than an if-statement because the players
        // velocity may exceed the length of several road segments. 
        else {



            // if(p->id == 3) {
            //     std::cout << "in else" << std::endl;

            //     std::cout << "dest->degree = " << dest->degree << std::endl;
            //     for(int i = 0; i < dest->degree; i++) {
            //         std::cout << "\tincident edge " << i << " " << *dest->edges[i] << std::endl;
            //     }
            //     std::cout << std::endl;
            // }

            while(tickDist >= targetDist) {
            

                int neid = rng->select(dest->degree);
                // Update position/velocity.

                nextEdge = dest->edges[neid];


                // if(p->id == 3) {
                //     std::cout << neid << " nextEdge: " << *nextEdge << std::endl;;
                //     std::cout << " dest: " << *dest << std::endl;;
                // }


                if(nextEdge->s->nid == dest->nid) {
                    fromStart = true;

                    // if(p->id == 3)
                    //     std::cout << "in if" << std::endl;

                    start = nextEdge->s;
                    dest = nextEdge->e;

                    // if(p->id == 3) {
                    //     std::cout << "new start = " << *start << std::endl;
                    //     std::cout << "new dest = " << *dest << std::endl;
                    // }
                }
                else {
                    fromStart = false;

                    // if(p->id == 3)
                    //     std::cout << "in else" << std::endl;

                    start = nextEdge->e;
                    dest = nextEdge->s;
                    
                    // if(p->id == 3) {
                    //     std::cout << "new start = " << *start << std::endl;
                    //     std::cout << "new dest = " << *dest << std::endl;
                    // }
                }
                
                tickDist = tickDist - targetDist;
                targetDist = nextEdge->length;
            }


            currEdge[i] = nextEdge;
            toEnd[i] = fromStart;

            // if(p->id == 3)
            //     std::cout << "---------------------------------" << std::endl;
            
            // Note: In this model, a car travels at the same speed through
            // all of the intersections and then changes speed for the next
            // tick. This isn't so realistic, but it's not clear what's a
            // better solution. 
            double v = speed[rng->uniformInt(0, numSpeeds)];
            double theta = atan2(dest->y - start->y, dest->x - start->x);
            newVx = v * cos(theta);
            newVy = v * sin(theta);

            double offset = tickDist / targetDist;
            newX = offset * (dest->x - start->x) + start->x;
            newY = offset * (dest->y - start->y) + start->y;


            //Position update
            //writer->writeUpdate(2, p->id, newX, newY, p->vx, p->vy, p->data);


            //Veloctiy update
            //writer->writeUpdate(0, p->id, newX, newY, newVx, newVy, p->data);


            // Combine position and velocity update to reduce update rate 
            writer->writeUpdate(2, p->id, newX, newY, newVx, newVy, p->data);


            numUpdates += 1;
        }

        if(newX < 0 || newY < 0) {
            std::cout << "MOVING NEGATIVE" << std::endl;
            std::cout << "dest: " << dest << std::endl;
        }


        p->x = newX;
        p->y = newY;
        p->vx = newVx;
        p->vy = newVy;


    }
    writer->endUpdates();
    writer->endTick();
}


void NetworkGenerator::genTrace() {
    openTrace();
    if(!nodefileSet || !edgefileSet) {
        throw std::logic_error("nodefile and edgefile must be set.");
    }

    speed = new double[numSpeeds];
    for(int i=0;i<numSpeeds;i++) {
        speed[i] = maxSpeed * (i+1) / numSpeeds;
    }

    network = new Network(nodefile, edgefile, space, rng);

    std::cout << "Read network..." << std::endl;

    genPoints();

    std::cout << "Generated points..." << std::endl;

    writer->writeHeader(numTicks);
    
    for(int i = 0; i < numTicks; i++) {
        genTick(i);
        //std::cout << "Generated Tick " << i << std::endl;

    }

    std::cout << "Num. Updates per Tick: " 
              << ((double) numUpdates / (double) numTicks) 
              << std::endl;

    closeTrace();
}

void NetworkGenerator::setParam(const char* param) {
    
    const char* v;

    if((v = testParam(param, "--nodefile=")) != 0) {
        nodefile = v;
        nodefileSet = true;
    }
    else if((v = testParam(param, "--edgefile=")) != 0) {
        edgefile = v;
        edgefileSet = true;
    }
    else if((v = testParam(param, "--space=")) != 0) {
        setSpace(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxSpeed=")) != 0) {
        setMaxSpeed(strtod(v, 0));
    }
    else if((v = testParam(param, "--numSpeeds=")) != 0) {
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
    else {
        DataGenerator::setParam(param);
    }

}

NetworkGenerator::NetworkGenerator() {
    //What's a good default for space? 
    this->space = 10000;
    this->maxSpeed = 100;
    this->numSpeeds = 10;
    this->qMin = 1000;
    this->qMax = 1000;
    this->probQuery = 1.0;
    this->numUpdates = 0;
}


NetworkGenerator::~NetworkGenerator() {
    delete [] speed;

    std::vector<Point2D*>::iterator it;
    for(it = points.begin(); it != points.end(); it++) {
        delete *it;
    }

    delete network;
}

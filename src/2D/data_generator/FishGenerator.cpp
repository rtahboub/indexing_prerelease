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

#include "2D/DataGenerator.h"
#include "2D/data_generator/FishGenerator.h"
#include "2D/data_generator/fish/oids.h"

#include "2D/data_generator/fish/trig.h"
#include "2D/data_generator/fish/unit.h"
#include "2D/data_generator/fish/FishRegion.h"
#include "2D/point_index/RTree.h"

#include "VectorGroupCallBack.h"

#include "util/RNG.h"
#include "util/Log.h"
#include "util/ptimer.h"

#include <cstdio>
#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace PSimIndex;


bool FishGenerator::firstidcomp(std::pair<Unit*, Unit*>* p1, 
                                std::pair<Unit*, Unit*>* p2) {
    int id11 = p1->first->getOid();
    int id12 = p1->second->getOid();
    int id21 = p2->first->getOid();
    int id22 = p2->second->getOid();

    return (id11 < id21 || (id11 == id21 && id12 < id22));
}


double FishGenerator::query() {
    DBUtl::PTimer pt;
    double indexTime = 0;


#ifdef WRITE_FISH_TRACE    
    writer->startQueries();
    writer->writeQuery(0, alpha, alpha);
#endif

    DBUtl::PTimer pt2;
    pt2.start();


    pt.start();

    //unitPosIndex->startTick(&unitSeq);

    UnitRG2D* rg = new UnitRG2D(alpha);
    VectorGroupCallBack<UnitKey, Unit, Unit> cb;

//    for(int i = 0; i < numPoints; i++) {
//        std::cout << i << " " << unitSeq[i]->getOid() << std::endl;
//    }

    unitPosIndex->enumerationJoin(&cb, &unitSeq, rg);



    pt.finish();
    indexTime += pt.realResult();


    // DBUtl::PTimer pt4;
    // pt4.start();

    // std::cout << "Printing cb..." << std::endl;
    // cb.print();

    //cb.sort(firstidcomp);
    //pt4.finish();
    //sortTime += pt4.realResult();

    //std::cout << "cb.size(): " << cb.size() << " sortTime: " <<  pt4.realResult() << " ";

    //JoinCallBack<Unit, Unit>::iterator cbItr = cb.begin();

    Unit *key, *value;
    std::vector<Unit*> rhoUnits;

    float alpha2 = alpha * alpha;
    float rho2 = rho * rho;

    VectorGroupCallBack<UnitKey, Unit, Unit>::val_iterator vit;
    VectorGroupCallBack<UnitKey, Unit, Unit>::key_iterator kit;
    
    for(int i = 1; i <= numPoints; i++) {
        //numMatches = 0;

        float qdx = (float) 0.0;
        float qdy = (float) 0.0;

        Unit* u = unitSeq[i-1];

        // No matches. Add unit to rhoUnits.
        if(cb.numMatches(unitSeq[i-1]) == 0) {
            rhoUnits.push_back(u);
        }
        else {
            for(vit = cb.matchBegin(u); vit != cb.matchEnd(u); vit++) {

                key = u;//(*cbItr)->first;
                value = *vit;//(*cbItr)->second;
            
                float xDiff = key->getX() - value->getX();
                float yDiff = key->getY() - value->getY();
                float normDiff2 = norm2(xDiff, yDiff);

                if(normDiff2 > 0 && normDiff2 < alpha2) {
                    qdx += -(xDiff / sqrt(normDiff2));
                    qdy += -(yDiff / sqrt(normDiff2));
                }
            }
            unitEffectsSeq[i-1]->updateDX(qdx);
            unitEffectsSeq[i-1]->updateDX(qdy);
        }

    }

    cb.clear();
    delete rg;

    pt2.finish();


#ifdef WRITE_FISH_TRACE    
    for(int i = 0; i < rhoUnits.size(); i++) {
        writer->addQuerier(rhoUnits[i]->getOid());
    }

    writer->writeQuery(0, rho, rho, 0);
#endif


    DBUtl::PTimer pt3;
    pt3.start();


    rg = new UnitRG2D(rho);

    pt.start();
    unitPosIndex->enumerationJoin(&cb, &rhoUnits, rg);
    pt.finish();
    indexTime += pt.realResult();

    //pt4.start();
    //cb.sort(firstidcomp);
    //pt4.finish();

    //sortTime += pt4.realResult();

    for(kit = cb.keyBegin(); kit != cb.keyEnd(); kit++) {
        float qdx = (float) 0.0;
        float qdy = (float) 0.0;
        
        Unit* u = *kit;


        for(vit = cb.matchBegin(*kit); vit != cb.matchEnd(*kit); vit++) {
            key = u;//(*cbItr)->first;
            value = *vit;//(*cbItr)->second;
            
            float xDiff = key->getX() - value->getX();
            float yDiff = key->getY() - value->getY();
            float normDiff2 = norm2(xDiff, yDiff);

            if(normDiff2 > 0 && normDiff2 < rho2) {

                if(value->getOid() != u->id) {
                    qdx += xDiff / sqrt(normDiff2);
                    qdy += yDiff / sqrt(normDiff2);
                }

                qdx += value->getVx();
                qdy += value->getVy();

            }
        }

        unitEffectsSeq[u->id - 1]->updateDX(qdx);
        unitEffectsSeq[u->id - 1]->updateDX(qdy);
    }

    // cbItr = cb.begin();

    // for(int i = 1; i <= numPoints; i++) {


    //     //For each match of agent i. 
    //     while(cbItr != cb.end() 
    //           && (*cbItr)->first->getOid() == i) {

    //         key = (*cbItr)->first;
    //         value = (*cbItr)->second;
            
    //         float xDiff = key->getX() - value->getX();
    //         float yDiff = key->getY() - value->getY();
    //         float normDiff2 = norm2(xDiff, yDiff);

    //         if(normDiff2 > 0 && normDiff2 < rho2) {

    //             if(value->getOid() != i) {
    //                 qdx += xDiff / sqrt(normDiff2);
    //                 qdy += yDiff / sqrt(normDiff2);
    //             }

    //             qdx += value->getVx();
    //             qdy += value->getVy();

    //         }

    //         cbItr++;
    //     }
    //     unitEffectsSeq[i-1]->updateDX(qdx);
    //     unitEffectsSeq[i-1]->updateDX(qdy);
    // }

    pt3.finish();
    //std::cout << " rho part: " << pt3.realResult() << " total: " << (pt2.realResult() + pt3.realResult()) << std::endl;

#ifdef WRITE_FISH_TRACE    
    writer->endQueries();
#endif

    cb.clear();

    delete rg;
    return indexTime;
}


void FishGenerator::update() {

#ifdef WRITE_FISH_TRACE    
    writer->startUpdates();
#endif

    // apply effects to state
    std::vector<UnitEffects*>::iterator effIt = unitEffectsSeq.begin();
    for (std::vector<Unit*>::iterator uit = unitSeq.begin(); uit != unitSeq.end(); uit++) {
        Unit* unit = *uit;
        UnitEffects* eff = *effIt;

        if (unit->getOid() != eff->getOid()) {
            throw new std::logic_error("invalid oids in unit and effect");
        } 

        float dX = eff->getDX();
        float dY = eff->getDY();

        if (dX != 0 || dY != 0) {
            // normalize d vector
            float dNorm = norm(dX, dY);
            dX = dX / dNorm;
            dY = dY / dNorm;


            // 2.5 Update the angle for informed individuals. 
            if(unit->getInformed()) {
                dX = dX + (Unit::OMEGA*unit->getGx());
                dY = dY + (Unit::OMEGA*unit->getGy());

                dNorm = norm(dX, dY);

                dX = dX / dNorm;
                dY = dY / dNorm;
            }
        }
        else {
            dX = unit->getVx();
            dY = unit->getVy();
        }


        // 3. Random rotation: rotate d by a random angle to simulate sensory/movement error
        float rAngle = (float) rng->gauss(Unit::GAUSS_MU, Unit::GAUSS_SIGMA);
        rotate(dX, dY, rAngle);

        // 4. Rotation limit: individuals can only rotate their angle by at most THETA per tick
        float rotAngle = angle(unit->getVx(), unit->getVy(), dX, dY);
        if (fabs(rotAngle) > Unit::THETA) {
            // use maximum rotation for d in the desired direction
            rotAngle = rotAngle >= 0? Unit::THETA : -Unit::THETA;
            dX = unit->getVx();
            dY = unit->getVy();
            rotate(dX, dY, rotAngle);
        } 


        // adjust direction of movement
        float newVx = (dX == 0 && dY == 0)? unit->getVx() : dX;
        float newVy = (dX == 0 && dY == 0)? unit->getVy() : dY;

        float oldVx = unit->getVx();
        float oldVy = unit->getVy();

        // update unit with effects
#ifdef WRITE_FISH_TRACE    
        writer->writeUpdate(0, unit->getOid(), 
                            unit->getX(), unit->getY(), 
                            newVx * unit->getSpeed(), 
                            newVy * unit->getSpeed(), 1.0);
#endif

        // sowell: Changed this to update the position using the old
        // velocity. This matches the index (and SGL) semantics. 
        float newX = unit->getX() + (oldVx) * unit->getSpeed();
        float newY = unit->getY() + (oldVy) * unit->getSpeed();


        unit->setX(newX);
        unit->setY(newY);



        if(unitPosIndex->isMovingJoin() && (newVx != oldVx || newVy != oldVy)) {
            MovingSpatialJoin2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>* mvJoin;
            mvJoin = (MovingSpatialJoin2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>*) unitPosIndex;

            mvJoin->beforeUpdateVelocity(unit, 
                                         unit->getX(), 
                                         unit->getY(), 
                                         newVx * unit->getSpeed(),
                                         newVy * unit->getSpeed());
        }


        unit->setVx(newVx);
        unit->setVy(newVy);


        // clear effect variables for next iteration
        eff->clear();
        effIt++;
    }


#ifdef WRITE_FISH_TRACE    
    writer->endUpdates();
#endif


}
void FishGenerator::genFish() {
    // initialize goal vectors
    goals.resize(numGoals);

    for (int i = 0; i < numGoals; i++) {
        float theta = (float)(rng->randf() * (2.0*M_PI));
        Goal* goal = new Goal(cos(theta), sin(theta));
        goals[i] = goal;
    }

    // generate random data to initilize units
    unitSeq.resize(numPoints);
    unitEffectsSeq.resize(numPoints);

    for (int i = 0; i < numPoints; i++) {

        // generate random coords and speeds

        float x = (float) rng->randf() * (maxX - minX) + minX;
        float y = (float) rng->randf() * (maxY - minY) + minY;
        float speed = rng->randf() * (maxSpeed - minSpeed) + minSpeed;
        
        float vx;
        float vy; 
        float norm;
        do {
            vx = (float)(2.0 * rng->randf() - 1.0);
            vy = (float)(2.0 * rng->randf() - 1.0);
            norm = sqrt(vx*vx + vy*vy);
        } while (norm == 0 || norm > 1);
        

        // normalize velocity   
        vx /= norm;
        vy /= norm;

        // decide whether the unit should be informed. 
        bool informed = (rng->randf() < pInformed) ? true : false;
        int goalNum = informed ? rng->uniformInt(0, goals.size()) : 0;

        // add unit
        int id = OidGenerator::getNextOid();
        // std::cout << "id: " << id << " numPoints: " << numPoints << std::endl;

        unitEffectsSeq[id-1] = new UnitEffects(id);


        unitSeq[i] = new Unit(id, x , y, vx, vy, 
                              speed, informed, 
                              goals[goalNum]->getGx(), 
                              goals[goalNum]->getGy());


#ifdef WRITE_FISH_TRACE    
        writer->addPoint(id, x, y, (vx * speed), (vy * speed), 1.0);
#endif
    }


#ifdef WRITE_FISH_TRACE    
    writer->writeHeader(numTicks);
#endif

}




void FishGenerator::setParam(const char* param) {

    const char* v;
    
    if((v = testParam(param, "--numGoals=")) != 0) {
        setNumGoals(strtol(v, 0, 0));
    }
    else if((v = testParam(param, "--minSpeed=")) != 0) {
        setMinSpeed(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxSpeed=")) != 0) {
        setMaxSpeed(strtod(v, 0));
    }
    else if((v = testParam(param, "--minX=")) != 0) {
        setMinX(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxX=")) != 0) {
        setMaxX(strtod(v, 0));
    }
    else if((v = testParam(param, "--minY=")) != 0) {
        setMinY(strtod(v, 0));
    }
    else if((v = testParam(param, "--maxY=")) != 0) {
        setMaxY(strtod(v, 0));
    }
    else if((v = testParam(param, "--alpha=")) != 0) {
        setAlpha(strtod(v, 0));
    }
    else if((v = testParam(param, "--rho=")) != 0) {
        setRho(strtod(v, 0));
    }
    else if((v = testParam(param, "--probInformed=")) != 0) {
        double pInformed = strtod(v, 0);
        setPInformed(pInformed);
        if(pInformed > 1) {
            throw std::logic_error("probInformed must be a probability");
        }
    }
    else {
        DataGenerator::setParam(param);
    }

}



void FishGenerator::genTrace() {
#ifdef WRITE_FISH_TRACE    
    openTrace();
#endif
    genFish();
    sortTime = 0.0;


    DBUtl::PTimer pt;
    
    double queryTime = 0;
    double updateTime = 0;
    double indexTime = 0;

    for(int i = 0; i < numTicks; i++) {
        
        // for(int j = 0; j < unitSeq.size(); j++) {
        //     std::cout << *unitSeq[j] << std::endl;
        // }

        unitPosIndex->startTick(&unitSeq);

        pt.start();
        indexTime += query();
        pt.finish();

        queryTime += pt.realResult();

        pt.start();
        update();
        pt.finish();

        updateTime += pt.realResult();

        unitPosIndex->endTick(i == numTicks-1);

#ifdef WRITE_FISH_TRACE    
        writer->endTick();
#endif




//        std::cout << "Generated tick " << i << std::endl;
    }

    ft = new FishTime(queryTime, updateTime, indexTime);

#ifdef WRITE_FISH_TRACE                
    closeTrace();
#endif
}

FishGenerator::FishGenerator(SpatialJoin2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>* unitJoin)
    : DataGenerator() {

    this->minX = 0; 
    this->maxX = 100000;
    this->minY = 0; 
    this->maxY = 100000;
    this->numGoals = 100;
    //TODO: Update to more realistic values;
    this->alpha = 25.0f;
    this->rho = 160.0f;
    this->pInformed = 0.15f;
            
    // RTree2D<Unit, UnitKey, UnitAgg2D>* rtree 
    //     = new RTree2D<Unit, UnitKey, UnitAgg2D>();

    unitPosIndex = unitJoin;


    // = new StaticNestedLoops2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>(rtree);
}





FishGenerator::~FishGenerator() {
    std::vector<Unit*>::iterator uit;
    std::vector<UnitEffects*>::iterator eit;

    // release all units
    for (uit = unitSeq.begin(); uit != unitSeq.end(); uit++) {
        delete (*uit);
    }
    // release all effects
    for (eit = unitEffectsSeq.begin(); eit != unitEffectsSeq.end(); eit++) {
        delete (*eit);
    }

    delete unitPosIndex;
    delete ft;

    OidGenerator::resetOid();

}



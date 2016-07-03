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

#ifndef _FISH_GENERATOR_
#define _FISH_GENERATOR_

#include "2D/TraceWriter2D.h"
#include "2D/DataGenerator.h"

#include "2D/data_generator/fish/oids.h"
#include "2D/data_generator/fish/unit.h"
#include "2D/data_generator/fish/trig.h"
#include "2D/data_generator/fish/unit.h"
#include "2D/data_generator/fish/FishRegion.h"

#include "2D/join_index/StaticNestedLoops2D.h"
#include "2D/join_index/MovingNestedLoops2D.h"
#include "2D/SpatialIndex2D.h"
#include "2D/SpatialJoin2D.h"


#include "util/RNG.h"
#include "util/Log.h"
#include "util/ptimer.h"

#include <vector>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <stdexcept>


namespace PSimIndex {

/*
 * Selection of index kind to use
 */

//#define INDEX_TYPE AggIndex2D_seminaive
//#define INDEX_NAME "seminaive"

//#define WRITE_FISH_TRACE 1


    /**
     * Unfortunately I don't know of a good way to do this with
     * TimingResult2D since there aren't 
     */
    struct FishTime {
        double queryTime;
        double updateTime; 
        double indexTime;

        FishTime(double query, double update, double index) 
            : queryTime(query), updateTime(update), indexTime(index)
        {}
    };

 
/**
 * Represents the simulation itself. In order to advance the simulation,
 * the tick method must be called externally. Internally, we will perform
 * both a query and an update step.
 *
 * TODO: Make this extend Workload2D
 */
    //template<template <class,class,class,class> class IndexType = StaticNestedLoops2D>
    class FishGenerator : public DataGenerator {

    private:
        std::vector<Unit*> unitSeq;
        std::vector<UnitEffects*> unitEffectsSeq;
        std::vector<Goal*> goals;

        int numGoals;
        float minX, maxX, minY, maxY;
        float alpha, rho, pInformed;
        float minSpeed, maxSpeed;

        SpatialJoin2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>* unitPosIndex;

        static bool firstidcomp(std::pair<Unit*, Unit*>* p1, 
                                std::pair<Unit*, Unit*>* p2);

        void genFish();
        double query();
        void update();

        inline std::vector<Unit*>& getUnits() { return unitSeq; }
        inline std::vector<Goal*>& getGoals() { return goals; }

        FishGenerator(SpatialJoin2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>* unitJoin);

        FishTime* ft;



    public:
        //TMP: For testing. 
        double sortTime;


        inline void setNumGoals(int numGoals) { this->numGoals = numGoals; }
        inline void setMinX(float minX) { this->minX = minX; }
        inline void setMaxX(float maxX) { this->maxX = maxX; }
        inline void setMinY(float minY) { this->minY = minY; }
        inline void setMaxY(float maxY) { this->maxY = maxY; }
        inline void setMinSpeed(float minSpeed) { this->minSpeed = minSpeed; }
        inline void setMaxSpeed(float maxSpeed) { this->maxSpeed = maxSpeed; }
        inline void setAlpha(float alpha) { this->alpha = alpha; }
        inline void setRho(float rho) { this->rho = rho; }
        inline void setPInformed(float pInformed) { 
            this->pInformed = pInformed; 
        }      

        void setParam(const char* param);
        FishTime* getFishTime() { return ft; }
        void genTrace();

        //template<template <class,class,class,class> class IndexType = StaticNestedLoops2D>
        template<template <class,class,class,class> class JoinType>
        static FishGenerator* createFishGenerator() {
            SpatialJoin2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>* unitJoin 
                = new JoinType<Unit,UnitKey,UnitAgg2D,UnitRG2D>();

            return new FishGenerator(unitJoin);
        }


        template<template <class,class,class> class IndexType>
        static FishGenerator* createFishGenerator() {
            IndexType<Unit,UnitKey,UnitAgg2D>* unitIndex = new IndexType<Unit,UnitKey,UnitAgg2D>();

            SpatialJoin2D<Unit, UnitKey, UnitAgg2D, UnitRG2D>* unitJoin; 

            if(unitIndex->isMovingIndex()) {
                MovingSpatialIndex2D<Unit,UnitKey,UnitAgg2D>* movingIndex
                    = (MovingSpatialIndex2D<Unit,UnitKey,UnitAgg2D>*) unitIndex;

                unitJoin = new MovingNestedLoops2D<Unit,UnitKey,UnitAgg2D,UnitRG2D>(movingIndex);
            }
            else {
                StaticSpatialIndex2D<Unit,UnitKey,UnitAgg2D>* staticIndex
                    = (StaticSpatialIndex2D<Unit,UnitKey,UnitAgg2D>*) unitIndex;

                unitJoin = new StaticNestedLoops2D<Unit,UnitKey,UnitAgg2D,UnitRG2D>(staticIndex);
            }

            return new FishGenerator(unitJoin);
        }



        ~FishGenerator();




    };
}

#endif

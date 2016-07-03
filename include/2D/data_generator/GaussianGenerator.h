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

#ifndef _GUASSIAN_GENERATOR_H_
#define _GUASSIAN_GENERATOR_H_

#include "2D/TestTypes2D.h"
#include "2D/DataGenerator.h"
#include "2D/TraceWriter2D.h"
#include <vector>
#include <iostream>

//Use gsl for gaussian inverse cdf.
//#include "DBUtl/include/RNG.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>


namespace PSimIndex {

    /**
     * Based on "A Benchmark for Evaluating Moving Object Indexes" 
     * by Chen et al. and the source available at
     *   http://www.comp.nus.edu.sg/~spade/benchmark
     *
     * TODO: Things to think about:
     *     - maxTup is related to insertTicks. Perhaps they should be the same? 
     *     - We may want to try distributing queries according to the 
     *       gaussians as well. 
     *     - What are good parameter settings?
     *     - Update TimingResult to truncate startup ticks. 
     */
    class GaussianGenerator : public DataGenerator {
    private:

        struct GaussUpdateRec {
            int type;
            int tick;
            int id;
            double x,y; 
            double vx, vy;
            bool insert;
            
            GaussUpdateRec(int tick, 
                           int id, 
                           double x, 
                           double y, 
                           double vx, 
                           double vy,
                           int type = 0) 
            : tick(tick), id(id), x(x), y(y), vx(vx), vy(vy), type(type)
            {}

            void print() {
                std::cout << type << " " << id << " " << tick << " "
                          << x << " " << y << " " 
                          << vx << " " << vy << std::endl;
            }

        };

        static bool compareUpdates(GaussUpdateRec* u1, GaussUpdateRec* u2);

        //For testing. Sorts by id first, then by time. 
        static bool testCompareUpdates(GaussUpdateRec* u1, GaussUpdateRec* u2);


        std::vector<Point2D*> points;
        std::vector<GaussUpdateRec*> updates;
        std::vector<Point2D*> hotspots;

        int numHotspots, numSpeeds, maxTup, insertTicks;
        double space, maxSpeed, qMin, qMax, probQuery;


        double radius;
        
        std::vector<double> ring;
        std::vector<double> speed;

        // TODO: This differs from Chen's code in that we use ints instead of
        // floats. Think about how to deal with rounding issues. 
        std::vector<int> tup;


        gsl_rng * r;

        double genSafePos(Point2D* p, int& spotNum);
        void genTickQueries(int tick);
        void genUpdates();

    public:


        inline void setSpace(double space) {this->space = space; }
        inline void setMaxSpeed(double maxSpeed) { this->maxSpeed = maxSpeed; }
        inline void setNumSpeeds(int numSpeed) { this->numSpeeds = numSpeeds; }
        inline void setNumHotspots(int numHotspots) { 
            this->numHotspots = numHotspots; 
        }
        inline void setInsertTicks(int insertTicks) { 
            this->insertTicks = insertTicks; 
        }
        inline void setMaxTup(int maxTup) { this->maxTup = maxTup; }        
        inline void setQMin(double qMin) { this->qMin = qMin; }
        inline void setQMax(double qMax) { this->qMax = qMax; }
        inline void setProbQuery(double probQuery) { 
            this->probQuery = probQuery; 
        }

        void setParam(const char* param);
        void genTrace();



        GaussianGenerator();
        ~GaussianGenerator();
    };



}



#endif /* _GUASSIAN_GENERATOR_H_ */

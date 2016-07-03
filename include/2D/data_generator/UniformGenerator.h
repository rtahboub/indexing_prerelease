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

#ifndef _UNIFORM_GENERATOR_H_
#define _UNIFORM_GENERATOR_H_

#include "2D/TestTypes2D.h"
#include "2D/DataGenerator.h"
//#include "2D/TraceWriter2D.h"
//#include "DBUtl/include/RNG.h"
#include <vector>
//#include <tr1/unordered_map>
//#include <tr1/unordered_set>
#include <unordered_map>
#include <unordered_set>

//testing
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>


namespace PSimIndex {
    class UniformGenerator : public DataGenerator {

    private:
        //std::vector<Point2D*> points;
//        std::tr1::unordered_map<int, Point2D*> pointMap;
    	std::unordered_map<int, Point2D*> pointMap;
        
        bool intTrace;
        bool contQueries;
        bool allInserts, allDeletes;

        double maxX, maxY;

        double minGenX, maxGenX, minGenY, maxGenY;

        double maxSpeed;
        int numSpeeds;

        double qMin, qMax;
        double probQuery, probUpdate;

        double* speed;

        Point2D* createPoint();

        // void genSafeVel(double x, double y, double& vx, double& vy);
        // void genSafePos(double &x, double &y, double vx, double vy);

        void writeIntUpdate(int type, int id, double x, double y, 
                            double vx, double vy, double data);

        void genPoints();
//        void genTick(std::tr1::unordered_set<int>* queriers);
        void genTick(std::unordered_set<int>* queriers);
    

        //testing
        gsl_rng * r;



    public:    
        inline void setInt(bool intTrace) { this->intTrace = intTrace; }
        inline void setAllDeletes(bool allDeletes) { this->allDeletes = allDeletes; }
        inline void setAllInserts(bool allInserts) { this->allInserts = allInserts; }
        inline void setCont(bool cont) { this->contQueries = cont; }


        inline void setMaxX(double maxX) { this->maxX = maxX; }
        inline void setMaxY(double maxY) { this->maxY = maxY; }

        inline void setMinGenX(double minGenX) { this->minGenX = minGenX; }
        inline void setMaxGenX(double maxGenX) { this->maxGenX = maxGenX; }
        inline void setMinGenY(double minGenY) { this->minGenY = minGenY; }
        inline void setMaxGenY(double maxGenY) { this->maxGenY = maxGenY; }


        inline void setMaxSpeed(double maxSpeed) { this->maxSpeed = maxSpeed; }

        inline void setNumSpeeds(int ns) { 
            //std::cout << "setting numSpeeds: " << numSpeeds << std::endl;
            this->numSpeeds = ns; 
            //std::cout << "set numSpeeds: " << this->numSpeeds << std::endl;

        }

        



        inline void setQMin(double qMin) { this->qMin = qMin; }
        inline void setQMax(double qMax) { this->qMax = qMax; }
        inline void setProbQuery(double probQuery) { 
            this->probQuery = probQuery; 
        }
        inline void setProbUpdate(double probUpdate) { 
            this->probUpdate = probUpdate; 
        }

        void genTrace();
        void setParam(const char* param);

        UniformGenerator();
        ~UniformGenerator();
    };

}
#endif /* _UNIFORM_GENERATOR_H_ */

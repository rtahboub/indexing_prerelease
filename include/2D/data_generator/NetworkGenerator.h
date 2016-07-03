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

#ifndef _NETWORK_GENERATOR_H_
#define _NETWORK_GENERATOR_H_

#include "2D/TestTypes2D.h"
#include "2D/DataGenerator.h"
#include "2D/data_generator/Network.h"
#include <vector>
#include <math.h>

namespace PSimIndex {
    class NetworkGenerator : public DataGenerator {
    private:
        std::vector<Point2D*> points;
        std::vector<Edge*> currEdge;
        std::vector<bool> toEnd;

        Network* network;
        const char* nodefile;
        const char* edgefile;

        double space;

        double qMin, qMax;
        double probQuery;

        double maxSpeed;
        int numSpeeds;
        double* speed;

        bool nodefileSet, edgefileSet;


        long numUpdates;


        inline double dist(double x1, double  y1, double x2, double y2) {
            return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
        }

    public:
        inline void setNodefile(const char* nodefile) {
            this->nodefile = nodefile;
            nodefileSet = true;
        }

        inline void setEdgefile(const char* edgefile) {
            this->edgefile = edgefile;
            edgefileSet = true;
        }

        inline void setSpace(double space) { this->space = space; }
        inline void setMaxSpeed(double maxSpeed) { this->maxSpeed = maxSpeed; }
        inline void setNumSpeeds(int numSpeed) { this->numSpeeds = numSpeeds; }
        inline void setQMin(double qMin) { this->qMin = qMin; }
        inline void setQMax(double qMax) { this->qMax = qMax; }
        inline void setProbQuery(double probQuery) { 
            this->probQuery = probQuery; 
        }

        void genPoints();
        void genTick(int tickNum);
        void genTrace();
        void setParam(const char* param);

        NetworkGenerator();
        ~NetworkGenerator();
    };



}


#endif /* _NETWORK_GENERATOR_H_ */

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

#ifndef _MOVING_INDEX_STRIPES_70000_POINTS_H_
#define _MOVING_INDEX_STRIPES_70000_POINTS_H_

#include "2D/Experiment2D.h"
#include "2D/point_index/TPRTree.h"
#include "2D/point_index/OptimizedStripes.h"
#include "2D/TestTypes2D.h"
#include "2D/workload/TraceWorkload2D.h"

namespace PSimIndex {
    class MovingIndexSTRIPES70000 : public Experiment2D {
    public:
        void init() {

            // nb: This places output files in the same place as the other
            // ScalePoints results for convenience while plotting. 
            folder = "out/traces/MovingIndex/MovingIndexScalePoints";
            experimentName = "MovingIndexScalePoints";
            addWorkloadParam("numPoints");

            int size = 26458;
            int numAgents = 10000;

            //TODO: replace with good bucket size. 
            

            JoinMethod* index = getJoinMethod(new Stripes<Point2D, Key2D, SumAgg2D>(200, 200, size, size));

            addIndex(index);


            // We artificially set the index num high so that it doesn't
            // conflict with the other indices. 
            indexNum[index->getName()] = 10;

            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints70000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints70000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints70000trial3.dat", true));
            addWorkload(workloads, "scalepoints70000", 0, 70000);
            workloadNum["scalepoints70000"] = 3;
        }


        MovingIndexSTRIPES70000() : Experiment2D() {}
    };
}

#endif /* _MOVING_INDEX_STRIPES_70000_POINTS_H_ */

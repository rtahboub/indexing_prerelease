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


#ifndef _STATIC_INDEX_SIMPLE_GRID_30000_POINTS_H_
#define _STATIC_INDEX_SIMPLE_GRID_30000_POINTS_H_ 

#include "2D/Experiment2D.h"
#include "2D/point_index/BinarySearch2D.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/point_index/SimpleGrid.h"
#include "2D/point_index/LinearizedKDTrie.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class StaticIndexSimpleGrid30000 : public Experiment2D {
    public:
        void init() {


            // nb: This places output files in the same place as the other
            // ScalePoints results for convenience while plotting. 
            folder = "out/traces/StaticIndex/StaticIndexScalePoints";
            experimentName = "StaticIndexScalePoints";
            addWorkloadParam("numPoints");

            int size = 17321;
            int numAgents = 30000;

            //TODO: replace with good bucket size. 
            
            JoinMethod* index = getJoinMethod(
                new SimpleGrid<Point2D, Key2D, SumAgg2D>(-1, size, 16, numAgents)
                );


            addIndex(index);

            // We artificially set the index num high so that it doesn't
            // conflict with the other indices. 
            indexNum[index->getName()] = 10;            

//             Workload2D* w = new TraceWorkload2D("traces/scalepoints30000.dat", true);
//             addWorkload(w, 0, 30000);
//             workloadNum[w->getName()] = 1;


            // Workload2D* w = new TraceWorkload2D("traces/points/scalepoints30000trial1.dat", true);
            // addWorkload(w, 0, 30000);
            // workloadNum[w->getName()] = 1;


            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints30000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints30000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints30000trial3.dat", true));
            addWorkload(workloads, "scalepoints30000", 0, 30000);
            workloadNum["scalepoints30000"] = 1;



        }


        StaticIndexSimpleGrid30000() : Experiment2D() {}
    };
}

#endif /* _STATIC_INDEX_SIMPLE_GRID_30000_POINTS_H_ */

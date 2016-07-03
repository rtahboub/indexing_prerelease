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

#ifndef _MOVING_INDEX_SCALE_UPDATES_H_
#define _MOVING_INDEX_SCALE_UPDATES_H_


#include "2D/Experiment2D.h"
#include "2D/point_index/TPRTree.h"
#include "2D/point_index/Stripes.h"
#include "2D/TestTypes2D.h"
#include "2D/workload/TraceWorkload2D.h"

namespace PSimIndex {
    class MovingIndexScaleUpdates : public Experiment2D {
    public:
        void init() {

            folder = "out/traces/MovingIndex/MovingIndexScaleUpdates";
            experimentName = "MovingIndexScaleUpdates";
            addWorkloadParam("UpdateFrac");

            int size = 22361;
            int numAgents = 50000;

            
	    addIndex(getJoinMethod(new Stripes<Point2D, Key2D, SumAgg2D>(200, 200, 22361, 22361)));
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D>()));


            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.1trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.1trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.1trial3.dat", true));
            addWorkload(workloads, "scaleupdates0.1", 0, 0.1);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.3trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.3trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.3trial3.dat", true));
            addWorkload(workloads, "scaleupdates0.3", 0, 0.3);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.5trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.5trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.5trial3.dat", true));
            addWorkload(workloads, "scaleupdates0.5", 0, 0.5);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.7trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.7trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.7trial3.dat", true));
            addWorkload(workloads, "scaleupdates0.7", 0, 0.7);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.9trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.9trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.9trial3.dat", true));
            addWorkload(workloads, "scaleupdates0.9", 0, 0.9);

        }

        MovingIndexScaleUpdates() : Experiment2D() {}
    };
}

#endif /* _MOVING_INDEX_SCALE_UPDATES_H_ */

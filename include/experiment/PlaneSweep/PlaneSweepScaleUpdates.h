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

#ifndef _PLANE_SWEEP_SCALE_UPDATES_H_
#define _PLANE_SWEEP_SCALE_UPDATES_H_

#include "2D/Experiment2D.h"
#include "2D/point_index/BinarySearch2D.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/TPRTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/point_index/LinearizedKDTrie.h"
#include "2D/point_index/Stripes.h"
#include "2D/workload/TraceWorkload2D.h"

#include "2D/join_index/GenericPlaneSweep2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/ForwardPlaneSweep.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"

#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class PlaneSweepScaleUpdates : public Experiment2D {
    public:
        void init() {


            folder = "out/traces/PlaneSweep/PlaneSweepScaleUpdates";
            experimentName = "PlaneSweepScaleUpdates";
            addWorkloadParam("UpdateFrac");

            addIndex(getJoinMethod(new ForwardPlaneSweep<Point2D, Key2D, SumAgg2D, RegionGen2D>()));

            SweepStructure<Point2D, Key2D>* listSweep = new ListSweep<Point2D,Key2D>();
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D, Key2D, SumAgg2D, RegionGen2D>(listSweep)));

            SweepStructure<Point2D, Key2D>* stripedSweep = new StripedSweep<Point2D,Key2D>(8);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));

            stripedSweep = new StripedSweep<Point2D,Key2D>(16);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));

            stripedSweep = new StripedSweep<Point2D,Key2D>(32);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));

            stripedSweep = new StripedSweep<Point2D,Key2D>(64);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));

            stripedSweep = new StripedSweep<Point2D,Key2D>(128);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));

            // addIndex(getJoinMethod(new ForwardPBSM2D<Point2D, Key2D, SumAgg2D, RegionGen2D>()));
            // addIndex(getJoinMethod(new GenericPBSM2D<Point2D, Key2D, SumAgg2D, RegionGen2D>()));

            double* param;

            // addWorkload(new TraceWorkload2D("traces/query0.5.update0.1large.dat", true), 0, 0.1);
            // addWorkload(new TraceWorkload2D("traces/query0.5.update0.3large.dat", true), 0, 0.3);
            // addWorkload(new TraceWorkload2D("traces/query0.5.update0.5large.dat", true), 0, 0.5);
            // addWorkload(new TraceWorkload2D("traces/query0.5.update0.7large.dat", true), 0, 0.7);
            // addWorkload(new TraceWorkload2D("traces/query0.5.update0.9large.dat", true), 0, 0.9);


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


        PlaneSweepScaleUpdates() : Experiment2D() {}
    };
}



#endif /* _PLANE_SWEEP_SCALE_UPDATES_H_ */

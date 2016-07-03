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

#ifndef _PLANE_SWEEP_SCALE_POINTS_H_
#define _PLANE_SWEEP_SCALE_POINTS_H_

#include "2D/Experiment2D.h"
#include "2D/point_index/BinarySearch2D.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/TPRTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/point_index/LinearizedKDTrie.h"
#include "2D/point_index/Stripes.h"
#include "2D/workload/TraceWorkload2D.h"

//#include "2D/join_index/CustomListSweep2D.h"
#include "2D/join_index/GenericPlaneSweep2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/ForwardPlaneSweep.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"

#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class PlaneSweepScalePoints : public Experiment2D {
    public:
        void init() {


            folder = "out/traces/PlaneSweep/PlaneSweepScalePoints";
            experimentName = "PlaneSweepScalePoints";
            addWorkloadParam("numPoints");

            addIndex(new ForwardPlaneSweep<Point2D, Key2D, SumAgg2D, RegionGen2D>());

            SweepStructure<Point2D, Key2D>* listSweep = new ListSweep<Point2D,Key2D>();
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(listSweep)));

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

            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints10000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints10000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints10000trial3.dat", true));
            addWorkload(workloads, "scalepoints10000", 0, 10000);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints30000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints30000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints30000trial3.dat", true));
            addWorkload(workloads, "scalepoints30000", 0, 30000);


            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial3.dat", true));
            addWorkload(workloads, "scalepoints50000", 0, 50000);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints70000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints70000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints70000trial3.dat", true));
            addWorkload(workloads, "scalepoints70000", 0, 70000);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints90000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints90000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/points/scalepoints90000trial3.dat", true));
            addWorkload(workloads, "scalepoints90000", 0, 90000);
        }


        PlaneSweepScalePoints() : Experiment2D() {}
    };
}







#endif /* _PLANE_SWEEP_SCALE_POINTS_H_ */

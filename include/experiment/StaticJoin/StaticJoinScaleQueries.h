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

#ifndef _STATIC_JOIN_SCALE_QUERIES_H_
#define _STATIC_JOIN_SCALE_QUERIES_H_

#include "2D/Experiment2D.h"

#include "2D/workload/TraceWorkload2D.h"

#include "2D/join_index/GenericPlaneSweep2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/ForwardPlaneSweep.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"
#include "2D/join_index/SynchTraversal.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class StaticJoinScaleQueries : public Experiment2D {
    public:
        void init() {


            folder = "out/traces/StaticJoin/StaticJoinScaleQueries";
            experimentName = "StaticJoinScaleQueries";
            addWorkloadParam("queryFrac");

            SweepStructure<Point2D, Key2D>* listSweep = new ListSweep<Point2D,Key2D>();
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(listSweep)));

            SweepStructure<Point2D, Key2D>* stripedSweep = new StripedSweep<Point2D,Key2D>(128);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));

            addIndex(getJoinMethod(new ForwardPBSM2D<Point2D, Key2D, SumAgg2D, RegionGen2D>()));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD)));

            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.1.update0.5trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.1.update0.5trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.1.update0.5trial3.dat", true));
            addWorkload(workloads, "scalequeries0.1", 0, 0.1);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.3.update0.5trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.3.update0.5trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.3.update0.5trial3.dat", true));
            addWorkload(workloads, "scalequeries0.3", 0, 0.3);


            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.5trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.5trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.5.update0.5trial3.dat", true));
            addWorkload(workloads, "scalequeries0.5", 0, 0.5);



            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.7.update0.5trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.7.update0.5trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.7.update0.5trial3.dat", true));
            addWorkload(workloads, "scalequeries0.7", 0, 0.7);


            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.9.update0.5trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.9.update0.5trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.9.update0.5trial3.dat", true));
            addWorkload(workloads, "scalequeries0.9", 0, 0.9);




        }


        StaticJoinScaleQueries() : Experiment2D() {}
    };
}


#endif /* _STATIC_JOIN_SCALE_QUERIES_H_ */

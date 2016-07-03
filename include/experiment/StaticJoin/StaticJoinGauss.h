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
        

#ifndef _STATIC_JOIN_GAUSS_H_
#define _STATIC_JOIN_GAUSS_H_

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
    class StaticJoinGauss : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/StaticJoin/StaticJoinGauss";
            experimentName = "StaticJoinGauss";
            addWorkloadParam("numHotSpots");

            SweepStructure<Point2D, Key2D>* listSweep = new ListSweep<Point2D,Key2D>();
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(listSweep)));

            SweepStructure<Point2D, Key2D>* stripedSweep = new StripedSweep<Point2D,Key2D>(128);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));

            addIndex(getJoinMethod(new ForwardPBSM2D<Point2D, Key2D, SumAgg2D, RegionGen2D>()));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD)));

            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1trial3.dat", true));
            addWorkload(workloads, "gauss1", 20, 1);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss10trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss10trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss10trial3.dat", true));
            addWorkload(workloads, "gauss10", 20, 10);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss100trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss100trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss100trial3.dat", true));
            addWorkload(workloads, "gauss100", 20, 100);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1000trial3.dat", true));
            addWorkload(workloads, "gauss1000", 20, 1000);

            // addWorkload(new TraceWorkload2D("traces/gauss/gauss1trial1.dat", true), 20, 1);
            // addWorkload(new TraceWorkload2D("traces/gauss/gauss10trial1.dat", true), 20, 10);
            // addWorkload(new TraceWorkload2D("traces/gauss/gauss100trial1.dat", true), 20, 100);
            // addWorkload(new TraceWorkload2D("traces/gauss/gauss1000trial1.dat", true), 20, 1000);
        }


        StaticJoinGauss() : Experiment2D() {}
    };
}


#endif /* _STATIC_JOIN_GAUSS_H_ */

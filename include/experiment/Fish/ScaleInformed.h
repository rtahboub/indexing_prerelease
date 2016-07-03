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

#ifndef _SCALE_INFORMED_H_
#define _SCALE_INFORMED_H_

#include "2D/Experiment2D.h"
#include "2D/point_index/CRTree.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/join_index/GenericPlaneSweep2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/ForwardPlaneSweep2D.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"
#include "2D/join_index/SynchTraversal.h"
#include "2D/point_index/OptimizedStripes.h"
#include "2D/TestTypes2D.h"

#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class ScaleInformed : public Experiment2D {
    public:
        void init() {

            folder = "out/traces/Fish/ScaleInformed";
            experimentName = "ScaleInformed";
            addWorkloadParam("FracInformed");

            addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D>()));

            SweepStructure<Point2D, Key2D>* stripedSweep = new StripedSweep<Point2D,Key2D>(128);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD)));
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D>()));
            addIndex(getJoinMethod(new OptimizedStripes<Point2D, Key2D, SumAgg2D>(12, 12, 4000, 4000)));
            // Simple grid needs to be run separately because we have to pass
            // the space parameters to the constructor. 

            // addIndex(getJoinMethod(new SimpleGrid2D<Point2D, Key2D, SumAgg2D>()));

            double* param;


            addWorkload(new TraceWorkload2D("traces/fishinformed0.1.dat", true), 0, 0.1);
            addWorkload(new TraceWorkload2D("traces/fishinformed0.3.dat", true), 0, 0.3);
            addWorkload(new TraceWorkload2D("traces/fishinformed0.5.dat", true), 0, 0.5);
            addWorkload(new TraceWorkload2D("traces/fishinformed0.7.dat", true), 0, 0.7);
            addWorkload(new TraceWorkload2D("traces/fishinformed0.9.dat", true), 0, 0.9);

            // addWorkload(new TraceWorkload2D("traces/scalepoints1000.dat", true), 0, 10000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints3000.dat", true), 0, 30000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints5000.dat", true), 0, 50000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints7000.dat", true), 0, 70000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0, 90000);

        }

        ScaleInformed() : Experiment2D() {}
    };
}

#endif /* _SCALE_INFORMED_H_ */

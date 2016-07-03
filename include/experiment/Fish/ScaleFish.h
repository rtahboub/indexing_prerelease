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

#ifndef _SCALE_FISH_H_
#define _SCALE_FISH_H_

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
    class ScaleFish : public Experiment2D {
    public:
        void init() {


            folder = "out/traces/Fish/ScaleFish";
            experimentName = "ScaleFish";
            addWorkloadParam("numFish");

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


            addWorkload(new TraceWorkload2D("traces/scalefish10000.dat", true), 0, 10000);
            addWorkload(new TraceWorkload2D("traces/scalefish30000.dat", true), 0, 30000);
            addWorkload(new TraceWorkload2D("traces/scalefish50000.dat", true), 0, 50000);
            addWorkload(new TraceWorkload2D("traces/scalefish70000.dat", true), 0, 70000);
            addWorkload(new TraceWorkload2D("traces/scalefish90000.dat", true), 0, 90000);

            // addWorkload(new TraceWorkload2D("traces/scalepoints1000.dat", true), 0, 10000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints3000.dat", true), 0, 30000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints5000.dat", true), 0, 50000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints7000.dat", true), 0, 70000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0, 90000);



        }

        
        ScaleFish() : Experiment2D() {}
    };
}

#endif /* _STATIC_INDEX_SCALE_POINTS_H_ */

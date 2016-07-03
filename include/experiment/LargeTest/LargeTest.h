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

#ifndef _LARGE_TEST_H_
#define _LARGE_TEST_H_

#include "2D/Experiment2D.h"
#include "2D/point_index/BinarySearch2D.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/point_index/SimpleGrid.h"
#include "2D/point_index/LinearizedKDTrie.h"

#include "2D/join_index/GenericPlaneSweep2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/ForwardPlaneSweep.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"
#include "2D/join_index/SynchTraversal.h"

#include "2D/point_index/TPRTree.h"
#include "2D/point_index/OptimizedStripes.h"


#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class LargeTest : public Experiment2D {
    public:

        void init() {

            folder = "out/traces/LargeTest/LargeTestScale";
            experimentName = "LargeTestScale";
            addWorkloadParam("numPoints");
            

            addIndex(getJoinMethod(new RTree<Point2D, Key2D, SumAgg2D>()));
            addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D>()));
            SweepStructure<Point2D, Key2D>* stripedSweep =  new StripedSweep<Point2D,Key2D>(128);
            addIndex(getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(stripedSweep)));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD)));


            addWorkload(new TraceWorkload2D("traces/points/scalepoints100000trial1.dat", true), 0, 100000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints300000trial1.dat", true), 0, 300000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints500000trial1.dat", true), 0, 500000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints700000trial1.dat", true), 0, 700000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints900000trial1.dat", true), 0, 900000);
        }


        LargeTest() : Experiment2D() {}
    };
}

#endif /* _LARGE_TEST_H_ */

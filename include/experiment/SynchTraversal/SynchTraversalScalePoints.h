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

#ifndef _SYNCH_TRAVERSAL_SCALE_POINTS_H_
#define _SYNCH_TRAVERSAL_SCALE_POINTS_H_

#include "2D/Experiment2D.h"
#include "2D/join_index/SynchTraversal.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class SynchTraversalScalePoints : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/SynchTraversal/SynchTraversalScalePoints";
            experimentName = "SynchTraversalScalePoints";
            addWorkloadParam("numPoints");


            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD)));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(RES)));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(SWEEP)));


            addWorkload(new TraceWorkload2D("traces/scalepoints1000.dat", true), 0, 1000);
            //addWorkload(new TraceWorkload2D("traces/scalepoints3000.dat", true), 3000);
            addWorkload(new TraceWorkload2D("traces/scalepoints5000.dat", true), 0, 5000);
            //addWorkload(new TraceWorkload2D("traces/scalepoints7000.dat", true), 7000);
            addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0, 9000);

            // addWorkload(new TraceWorkload2D("traces/scalepoints10000.dat", true), 0, 10000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints30000.dat", true), 0, 30000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints50000.dat", true), 0, 50000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints70000.dat", true), 0, 70000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints90000.dat", true), 0, 90000);

        }

        SynchTraversalScalePoints() : Experiment2D() {}
    };
}

#endif /* _SYNCH_TRAVERSAL_SCALE_POINTS_H_ */

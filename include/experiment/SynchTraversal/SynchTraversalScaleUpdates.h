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

#ifndef _SYNCH_TRAVERSAL_SCALE_UPDATES_H_
#define _SYNCH_TRAVERSAL_SCALE_UPDATES_H_

#include "2D/Experiment2D.h"
#include "2D/join_index/SynchTraversal.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class SynchTraversalScaleUpdates : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/SynchTraversal/SynchTraversalScaleUpdates";
            experimentName = "SynchTraversalScaleUpdates";
            addWorkloadParam("UpdateFrac");

            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD)));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(RES)));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(SWEEP)));

            addWorkload(new TraceWorkload2D("traces/query0.5.update0.1large.dat", true), 0, 0.1);
            //addWorkload(new TraceWorkload2D("traces/query0.5.update0.1small.dat", true), 0.1);

            addWorkload(new TraceWorkload2D("traces/query0.5.update0.3large.dat", true), 0, 0.3);
            //addWorkload(new TraceWorkload2D("traces/query0.5.update0.3small.dat", true), 0.3);

            addWorkload(new TraceWorkload2D("traces/query0.5.update0.5large.dat", true), 0, 0.5);
            //addWorkload(new TraceWorkload2D("traces/query0.5.update0.5small.dat", true), 0.5);

            addWorkload(new TraceWorkload2D("traces/query0.5.update0.7large.dat", true), 0, 0.7);
            //addWorkload(new TraceWorkload2D("traces/query0.5.update0.7small.dat", true), 0.7);

            addWorkload(new TraceWorkload2D("traces/query0.5.update0.9large.dat", true), 0, 0.9);
            //addWorkload(new TraceWorkload2D("traces/query0.5.update0.9small.dat", true), 0.9);
        }

        SynchTraversalScaleUpdates() : Experiment2D() {}
    };
}



#endif /* _SYNCH_TRAVERSAL_SCALE_UPDATES_H_ */

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

#ifndef _SCALE_NETWORK_H_
#define _SCALE_NETWORK_H_

#include "2D/Experiment2D.h"
#include "2D/point_index/CRTree.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/join_index/GenericPlaneSweep2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/ForwardPlaneSweep.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"
#include "2D/join_index/SynchTraversal.h"
#include "2D/point_index/Stripes.h"
#include "2D/TestTypes2D.h"

#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class ScaleNetwork : public Experiment2D {
    public:
        void init() {

            folder = "out/traces/Network";
            experimentName = "Network";


            addIndex(getJoinMethod(new RTree<Point2D, Key2D, SumAgg2D>()));
            addIndex(getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD)));
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D>()));
            addIndex(getJoinMethod(new Stripes<Point2D, Key2D, SumAgg2D>(101, 101, 100000, 100000)));


            addWorkload(new TraceWorkload2D("traces/olnetwork.dat", true), 0);
	    addWorkload(new TraceWorkload2D("traces/sgnetwork.dat", true), 0); 
	    addWorkload(new TraceWorkload2D("traces/sanetwork.dat", true), 0); 

        }

        
        ScaleNetwork() : Experiment2D() {}
    };
}

#endif /* _STATIC_INDEX_SCALE_POINTS_H_ */

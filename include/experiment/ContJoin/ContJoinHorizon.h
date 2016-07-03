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

#ifndef _CONT_JOIN_HORIZON_H_
#define _CONT_JOIN_HORIZON_H_

#include "2D/Experiment2D.h"
#include "2D/continuous_join/AEJoin.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class ContJoinHorizon : public Experiment2D {
    public:
        void init() {

            folder = "out/traces/ContJoin/ContJoinHorizon";
            experimentName = "ContJoinHorizon";
            addIndexParam("EventGenCycle");
            
            for(int i = 2; i < 100; i += 20) {
                JoinMethod* index = getJoinMethod(new AEJoin<Point2D, Key2D, SumAgg2D, RegionGen2D>(i));
                addIndex(index, i);
            }

            addWorkload(new TraceWorkload2D("traces/query_update/largequery0.0001.update0.001cont.dat", true), 0);
            //addWorkload(new TraceWorkload2D("traces/aework.dat", true), 0);
            //addWorkload(new TraceWorkload2D("traces/points/scalepoints50000conttrial1.dat", true), 0);
            
            // std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            // workloads->push_back(new TraceWorkload2D("traces/points/scalepoints500conttrial1.dat", true));
            // workloads->push_back(new TraceWorkload2D("traces/points/scalepoints500conttrial2.dat", true));
            // workloads->push_back(new TraceWorkload2D("traces/points/scalepoints500conttrial3.dat", true));
            // addWorkload(workloads, "scalepoints500", 0);
        }

        ContJoinHorizon() : Experiment2D() {}
    };
}

#endif /* _CONT_JOIN_HORIZON_H_ */

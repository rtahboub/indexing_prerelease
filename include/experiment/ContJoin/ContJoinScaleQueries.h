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

#ifndef _CONT_JOIN_SCALE_QUERIES_H_
#define _CONT_JOIN_SCALE_QUERIES_H_


#include "2D/Experiment2D.h"
#include "2D/point_index/TPRTree.h"
#include "2D/continuous_join/AEJoin.h"
#include "2D/TestTypes2D.h"
#include "2D/workload/TraceWorkload2D.h"

namespace PSimIndex {
    class ContJoinScaleQueries : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/ContJoin/ContJoinScaleQueries";
            experimentName = "ContJoinScaleQueries";
            addWorkloadParam("QueryFrac");

            addIndex(getJoinMethod(new AEJoin<Point2D, Key2D, SumAgg2D, RegionGen2D>()));

            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.1.update0.5conttrial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.1.update0.5conttrial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.1.update0.5conttrial3.dat", true));
            addWorkload(workloads, "scalequeries0.1", 0, 0.1);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.3.update0.5conttrial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.3.update0.5conttrial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/query_update/query0.3.update0.5conttrial3.dat", true));
            addWorkload(workloads, "scalequeries0.3", 0, 0.3);

            // addWorkload(new TraceWorkload2D("traces/query_update/query0.1.update0.5conttrial1.dat", true), 0, 0.1);
            // addWorkload(new TraceWorkload2D("traces/query_update/query0.3.update0.5conttrial1.dat", true), 0, 0.3);
            // addWorkload(new TraceWorkload2D("traces/query_update/query0.5.update0.5conttrial1.dat", true), 0, 0.5);
            // addWorkload(new TraceWorkload2D("traces/query_update/query0.7.update0.5conttrial1.dat", true), 0, 0.7);
            // addWorkload(new TraceWorkload2D("traces/query_update/query0.9.update0.5conttrial1.dat", true), 0, 0.9);
        }

        
        ContJoinScaleQueries() : Experiment2D() {}
    };
}

#endif /* _CONT_JOIN_SCALE_QUERIES_H_ */

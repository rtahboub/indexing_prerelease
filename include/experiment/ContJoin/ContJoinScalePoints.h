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

#ifndef _CONT_JOIN_SCALE_POINTS_H_
#define _CONT_JOIN_SCALE_POINTS_H_


#include "2D/Experiment2D.h"
#include "2D/point_index/TPRTree.h"
#include "2D/continuous_join/AEJoin.h"
#include "2D/TestTypes2D.h"
#include "2D/workload/TraceWorkload2D.h"

namespace PSimIndex {
    class ContJoinScalePoints : public Experiment2D {
    public:
        void init() {

            folder = "out/traces/ContJoin/ContJoinScalePoints";
            experimentName = "ContJoinScalePoints";
            addWorkloadParam("NumPoints");

            addIndex(getJoinMethod(new AEJoin<Point2D, Key2D, SumAgg2D, RegionGen2D>()));

            addWorkload(new TraceWorkload2D("traces/points/scalepoints10000conttrial1.dat", true), 0, 10000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints30000conttrial1.dat", true), 0, 30000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints50000conttrial1.dat", true), 0, 50000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints70000conttrial1.dat", true), 0, 70000);
            addWorkload(new TraceWorkload2D("traces/points/scalepoints90000conttrial1.dat", true), 0, 90000);

        }

        
        ContJoinScalePoints() : Experiment2D() {}
    };
}

#endif /* _CONT_JOIN_SCALE_POINTS_H_ */

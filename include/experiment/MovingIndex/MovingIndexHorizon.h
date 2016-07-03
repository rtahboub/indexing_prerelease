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

#ifndef _MOVING_INDEX_HORIZON_H_
#define _MOVING_INDEX_HORIZON_H_

#include "2D/point_index/OptimizedStripes.h"
#include "2D/point_index/TPRTree.h"
#include "2D/Experiment2D.h"

namespace PSimIndex {
    class MovingIndexHorizon : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/MovingIndex/MovingIndexHorizon/";
            experimentName = "MovingIndexHorizon";
            addIndexParam("Horizon");

            for(int i = 2; i <= 60; i += 10) {
                addIndex(getJoinMethod(new Stripes<Point2D, Key2D, SumAgg2D>(200, 200, 22361, 22361, i)), i);
                addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D>(i, false, false)), i);
            }
            //addWorkload(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true), 0);
	    // addWorkload(new TraceWorkload2D("traces/points/scalepoints1000.dat", true), 0);


            std::vector<Workload2D*>* wc = new std::vector<Workload2D*>();
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial2.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial3.dat", true));

            addWorkload(wc, "scalepoints50000", 0);


        }

        MovingIndexHorizon() : Experiment2D() {}
    };
}
#endif /* _MOVING_INDEX_HORIZON_H_ */

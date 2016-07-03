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

#ifndef _STRIPES_HORIZON_HUGE_H_
#define _STRIPES_HORIZON_HUGE_H_

#include "2D/point_index/OptimizedStripes.h"
#include "2D/Experiment2D.h"

namespace PSimIndex {
    class STRIPESHorizonHuge : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/STRIPES/STRIPESHorizonHuge/";
            experimentName = "STRIPESHorizonHuge";
            addIndexParam("Horizon");

            for(int i = 5; i <= 60; i += 5) {
                addIndex(getJoinMethod(new OptimizedStripes<Point2D, Key2D, SumAgg2D>(666, 666, 577350, 577350, i)), i);
            }

            addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0);

        }

        STRIPESHorizonHuge() : Experiment2D() {}
    };
}

#endif /* _STRIPES_HORIZON_HUGE_H_ */

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

#ifndef _STRIPES_HORIZON_SMALL_H_
#define _STRIPES_HORIZON_SMALL_H_

#include "2D/point_index/OptimizedStripes.h"
#include "2D/Experiment2D.h"


namespace PSimIndex {
    class STRIPESHorizonSmall : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/STRIPES/STRIPESHorizonSmall/";        
            experimentName = "STRIPESHorizonSmall";
            addIndexParam("Horizon");

            for(int i = 5; i <= 60; i += 5) {
                addIndex(getJoinMethod(new OptimizedStripes<Point2D, Key2D, SumAgg2D>(45, 45, 3000, 3000, i)), i);
            }

            addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0);

        }

        STRIPESHorizonSmall() : Experiment2D() {}
    };
}

#endif /* _STRIPES_HORIZON_SMALL_H_ */

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

#ifndef _STRIPES_HORIZON_GAUSS_H_
#define _STRIPES_HORIZON_GAUSS_H_

#include "2D/point_index/OptimizedStripes.h"
#include "2D/Experiment2D.h"

namespace PSimIndex {
    class STRIPESHorizonGauss : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/STRIPES/STRIPESHorizonGauss/";
            experimentName = "STRIPESHorizonGauss";
            addIndexParam("Horizon");

            for(int i = 5; i <= 60; i += 10) {
                addIndex(getJoinMethod(new OptimizedStripes<Point2D, Key2D, SumAgg2D>(200, 200, 22361, 22361, i)), i);
                addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D>(i)), i);

            }

            addWorkload(new TraceWorkload2D("traces/gauss1.dat", true), 0);

        }

        STRIPESHorizonGauss() : Experiment2D() {}
    };
}

#endif /* _STRIPES_HORIZON_GAUSS_H_ */

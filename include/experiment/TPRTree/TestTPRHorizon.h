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

#ifndef _TEST_TPR_HORIZON_H_
#define _TEST_TPR_HORIZON_H_

#include "2D/point_index/TPRTree.h"

namespace PSimIndex {
    
    class TestTPRHorizon : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/TPRTree/TestTPRHorizon/";
            experimentName = "TestTPRHorizon";
            addIndexParam("Horizon");

            double* param;


            for(int i = 5; i <= 60; i += 5) {
                param = new double[i];
                param[0] = i;

                addIndex(
                    getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D>(i)),
                    param);

            }

            addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0);
//            addWorkload(new TraceWorkload2D("traces/scalepoints90000.dat", true), 0);
            //addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0);
//            addWorkload(new TraceWorkload2D("traces/gauss1.dat", true), 20);

            //addWorkload(new TraceWorkload2D("traces/scalepoints10000.dat", true));
        }

        TestTPRHorizon() : Experiment2D() {}
    };
}

#endif /* _TEST_TPR_HORIZON_H_ */

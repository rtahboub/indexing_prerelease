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

#ifndef _TEST_TPR_ENTRIES_H_
#define _TEST_TPR_ENTRIES_H_

#include "2D/point_index/TPRTree.h"
//#include "2D/point_index/TPRTreeEntry2D.h"

namespace PSimIndex {

    class TestTPREntries : public Experiment2D {
    public:

        void init() {
            // folder = "out/traces/TPRTree/TestTPREntries/";
            // experimentName = "TestTPREntries";
            // addWorkloadParam("numPoints");

            // double* param;

            
            // addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D>()));
            // addIndex(getJoinMethod(new TPRTreeEntry<Point2D, Key2D, SumAgg2D>()));


            // addWorkload(new TraceWorkload2D("traces/scalepoints10000.dat", true), 0, 10000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints30000.dat", true), 0, 30000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints50000.dat", true), 0, 50000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints70000.dat", true), 0, 70000);
            // addWorkload(new TraceWorkload2D("traces/scalepoints90000.dat", true), 0, 90000);

        }



        TestTPREntries() : Experiment2D() {}

    };



}


#endif /* _TEST_TPR_ENTRIES_H_ */

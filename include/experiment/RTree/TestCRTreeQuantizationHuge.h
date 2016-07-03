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

#ifndef _TEST_CRTREE_QUANTIZATION_HUGE_H_
#define _TEST_CRTREE_QUANTIZATION_HUGE_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/BinarySearch2D.h"
#include "2D/join_index/PBSM2D.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/TPRTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/point_index/LinearizedKDTrie.h"
#include "2D/point_index/OrthogonalRangeTree2D.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/join_index/LinearizedKDTrieSweepLine.h"
#include "2D/join_index/SSSJ.h"

#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class TestCRTreeQuantizationHuge : public Experiment2D {
    public:
        void init() {

            addIndexParam("NumEntries");
            folder = "out/traces/RTree/TestCRTreeQuantizationHuge/";
            experimentName = "TestCRTreeQuantizationHuge";
            
            for(int i = 2; i < 101; i += 4) {
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint8_t>(i)), i);
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint16_t>(i)), i);
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint32_t>(i)), i);
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint64_t>(i)), i);
                addIndex(getJoinMethod(new RTree<Point2D, Key2D, SumAgg2D>(i)), i);
            }

            addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0);

        }

        TestCRTreeQuantizationHuge() : Experiment2D() {}
    };
}

#endif /* _TEST_CRTREE_QUANTIZATION_HUGE_H_ */

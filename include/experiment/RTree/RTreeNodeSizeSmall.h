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


#ifndef _RTREE_NODE_SIZE_SMALL_H_
#define _RTREE_NODE_SIZE_SMALL_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class RTreeNodeSizeSmall : public Experiment2D {
    public:
        void init() {

            addIndexParam("EntrySize");
            folder = "out/traces/RTree/RTreeNodeSizeSmall/";
            experimentName = "RTreeNodeSizeSmall";

            for(int i = 0; i < 73; i += 4) {
                addIndex(getJoinMethod(new RTree<Point2D, Key2D, SumAgg2D>(i+2)), 88 + 40*i);
            }

            for(int i = 0; i < 245; i += 12) {
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint8_t>(i+2)), 64 + 12*i);
            }

            for(int i = 0; i < 183; i += 8) {
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint16_t>(i+2)), 72 + 16*i);
            }

            for(int i = 0; i < 122; i += 6) {
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint32_t>(i+2)), 88 + 24*i);
            }

            addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0);

        }

        RTreeNodeSizeSmall() : Experiment2D() {}
    };
}

#endif /* _RTREE_NODE_SIZE_SMALL_H_ */

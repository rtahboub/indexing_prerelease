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

#ifndef _TPR_TREE_NODE_MIN_H_
#define _TPR_TREE_NODE_MIN_H_


#include "2D/point_index/TPRTree.h"

namespace PSimIndex {
    
    class TPRTreeNodeMin : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/TPRTree/TPRTreeNodeMin/";
            experimentName = "TPRTreeNodeMin";
            addIndexParam("MinNodeSize");

            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 1, 36>()), 36 + 76*1);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 5, 36>()), 36 + 76*5);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 10, 36>()), 36 + 76*10);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 15, 36>()), 36 + 76*15);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 20, 36>()), 36 + 76*20);


            addWorkload(new TraceWorkload2D("traces/scalepoints50000.dat", true), 0);
        }

        TPRTreeNodeMin() : Experiment2D() {}
    };
}


#endif /* _TPR_TREE_NODE_MIN_H_ */

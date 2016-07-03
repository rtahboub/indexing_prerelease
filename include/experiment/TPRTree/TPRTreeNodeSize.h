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

#ifndef _TPR_TREE_NODE_SIZE_H_
#define _TPR_TREE_NODE_SIZE_H_


#include "2D/point_index/TPRTree.h"

namespace PSimIndex {
    
    class TPRTreeNodeSize : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/TPRTree/TPRTreeNodeSize/";
            experimentName = "TPRTreeNodeSize";
            addIndexParam("NodeSize");

            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 1, 4>()), 36 + 76*4);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 1, 20>()), 36 + 76*20);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 1, 36>()), 36 + 76*36);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 1, 52>()), 36 + 76*52);
            addIndex(getJoinMethod(new TPRTree<Point2D, Key2D, SumAgg2D, 1, 68>()), 36 + 76*68);

            std::vector<Workload2D*>* wc = new std::vector<Workload2D*>();
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial2.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial3.dat", true));

            addWorkload(wc, "scalepoints50000", 0);
        }

        TPRTreeNodeSize() : Experiment2D() {}
    };
}


#endif /* _TPR_TREE_NODE_SIZE_H_ */

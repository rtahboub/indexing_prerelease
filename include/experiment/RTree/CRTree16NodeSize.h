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


#ifndef _CRTREE_16_NODE_SIZE_H_
#define _CRTREE_16_NODE_SIZE_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class CRTree16NodeSize : public Experiment2D {
    public:
        void init() {

            addIndexParam("EntrySize");
            folder = "out/traces/RTree/CRTree16NodeSize/";
            experimentName = "CRTree16NodeSize";

            for(int i = 2; i < 183; i += 20) {
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D, uint16_t>(i)), 64 + 16*i);
            }

            //addWorkload(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true), 0);            



            std::vector<Workload2D*>* wc = new std::vector<Workload2D*>();
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial2.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial3.dat", true));

            addWorkload(wc, "scalepoints50000", 0);

            // addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0);            
            // addWorkload(new TraceWorkload2D("traces/scalepoints90000.dat", true), 0);            
            // addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0) ;
            // addWorkload(new TraceWorkload2D("traces/gauss1.dat", true), 20);
        }

        CRTree16NodeSize() : Experiment2D() {}
    };
}

#endif /* _CRTREE_16_NODE_SIZE_H_ */

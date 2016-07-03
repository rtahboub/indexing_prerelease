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


#ifndef _FILL_FACTOR_H_
#define _FILL_FACTOR_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class FillFactor : public Experiment2D {
    public:
        void init() {

            addIndexParam("FillFactor");
            folder = "out/traces/RTree/FillFactor/";
            experimentName = "FillFactor";

            for(double ff = 0.5; ff < 1.05; ff += 0.1) {
                addIndex(getJoinMethod(new RTree<Point2D, Key2D, SumAgg2D>(12, ff)), ff);
                addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D>(28, ff)), ff);
            }

            std::vector<Workload2D*>* wc = new std::vector<Workload2D*>();
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints500000trial.dat", true));
            //wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial2.dat", true));
            //wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial3.dat", true));
            addWorkload(wc->at(0),/* "scalepoints50000", */0);
        }
        
        FillFactor() : Experiment2D() {}
    };
}

#endif /* _FILL_FACTOR_H_ */

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

#ifndef _NUM_GRID_CELLS_LARGE_H_
#define _NUM_GRID_CELLS_LARGE_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/SimpleGrid.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {

    class NumGridCellsLarge : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/SimpleGrid/NumGridCellsLarge";
            experimentName = "NumGridCellsLarge";
            addIndexParam("GridCellsPerSide");
            //addWorkloadParam("NumPoints");

            double* param;

            int size = 22361;


            for(int i = 1; i < 40; i += 4) {
                addIndex(getJoinMethod(new SimpleGrid<Point2D, Key2D, SumAgg2D>(i, size)), i);
            }

            std::vector<Workload2D*>* wc = new std::vector<Workload2D*>();
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial2.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial3.dat", true));

            addWorkload(wc, "scalepoints50000", 0);

        }
        
        NumGridCellsLarge() : Experiment2D() {}
    };
}

#endif /* _NUM_GRID_CELLS_LARGE_H_ */

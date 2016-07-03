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

#ifndef _NUM_GRID_CELLS_HUGE_H_
#define _NUM_GRID_CELLS_HUGE_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/SimpleGrid.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {

    class NumGridCellsHuge : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/SimpleGrid/NumGridCellsHuge";
            experimentName = "NumGridCellsHuge";
            addIndexParam("GridCellsPerSide");
            //addWorkloadParam("NumPoints");

            double* param;

            int size = 577350;

            // //TODO: Unfortunately, I don't know how to do this without manually unrolling the loop.
            // SimpleGrid<Point2D,Key2D,SumAgg2D,1>* grid1 = new SimpleGrid<Point2D,Key2D,SumAgg2D,1>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,2>* grid2 = new SimpleGrid<Point2D,Key2D,SumAgg2D,2>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,3>* grid3 = new SimpleGrid<Point2D,Key2D,SumAgg2D,3>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,4>* grid4 = new SimpleGrid<Point2D,Key2D,SumAgg2D,4>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,5>* grid5 = new SimpleGrid<Point2D,Key2D,SumAgg2D,5>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,6>* grid6 = new SimpleGrid<Point2D,Key2D,SumAgg2D,6>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,7>* grid7 = new SimpleGrid<Point2D,Key2D,SumAgg2D,7>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,8>* grid8 = new SimpleGrid<Point2D,Key2D,SumAgg2D,8>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,9>* grid9 = new SimpleGrid<Point2D,Key2D,SumAgg2D,9>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,10>* grid10 = new SimpleGrid<Point2D,Key2D,SumAgg2D,10>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,11>* grid11 = new SimpleGrid<Point2D,Key2D,SumAgg2D,11>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,12>* grid12 = new SimpleGrid<Point2D,Key2D,SumAgg2D,12>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,13>* grid13 = new SimpleGrid<Point2D,Key2D,SumAgg2D,13>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,14>* grid14 = new SimpleGrid<Point2D,Key2D,SumAgg2D,14>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,15>* grid15 = new SimpleGrid<Point2D,Key2D,SumAgg2D,15>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,16>* grid16 = new SimpleGrid<Point2D,Key2D,SumAgg2D,16>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,17>* grid17 = new SimpleGrid<Point2D,Key2D,SumAgg2D,17>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,18>* grid18 = new SimpleGrid<Point2D,Key2D,SumAgg2D,18>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,19>* grid19 = new SimpleGrid<Point2D,Key2D,SumAgg2D,19>(size);
            // SimpleGrid<Point2D,Key2D,SumAgg2D,20>* grid20 = new SimpleGrid<Point2D,Key2D,SumAgg2D,20>(size);


            for(int i = 1; i < 20; i += 2) {
                addIndex(getJoinMethod(new SimpleGrid<Point2D,Key2D,SumAgg2D>(i, size)), i);
            }

            // // param = new double[1];
            // // param[0] = 1;
            // addIndex(getJoinMethod(grid1), 1);
            // //addIndex(getJoinMethod(grid2), 2);
            // addIndex(getJoinMethod(grid3), 3);
            // //addIndex(getJoinMethod(grid4), 4);
            // addIndex(getJoinMethod(grid5), 5);
            // //addIndex(getJoinMethod(grid6), 6);
            // addIndex(getJoinMethod(grid7), 7);
            // //addIndex(getJoinMethod(grid8), 8);
            // addIndex(getJoinMethod(grid9), 9);
            // //addIndex(getJoinMethod(grid10), 10);
            // addIndex(getJoinMethod(grid11), 11);
            // //addIndex(getJoinMethod(grid12), 12);
            // addIndex(getJoinMethod(grid13), 13);
            // //addIndex(getJoinMethod(grid14), 14);
            // addIndex(getJoinMethod(grid15), 15);
            // //addIndex(getJoinMethod(grid16), 16);
            // addIndex(getJoinMethod(grid17), 17);
            // //addIndex(getJoinMethod(grid18), 18);
            // addIndex(getJoinMethod(grid19), 19);
            // //addIndex(getJoinMethod(grid20), 20);


            addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0);

        }
        
        NumGridCellsHuge() : Experiment2D() {}
    };
}

#endif /* _NUM_GRID_CELLS_HUGE_H_ */

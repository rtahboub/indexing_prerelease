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

#ifndef _SIMPLE_GRID_BUCKET_SIZE_GAUSS_H_
#define _SIMPLE_GRID_BUCKET_SIZE_GAUSS_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/SimpleGrid.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class SimpleGridBucketSizeGauss : public Experiment2D {
    public:

        void init() {
            folder = "out/traces/SimpleGrid/SimpleGridBucketSizeGauss";
            experimentName = "SimpleGridBucketSizeGauss";
            addIndexParam("bucketSize");


            int size = 22361;
            int numAgents = 50000;

            for(int i = 4; i <= 32; i += 4) {
                SimpleGrid<Point2D,Key2D,SumAgg2D>* grid;
                grid = new SimpleGrid<Point2D,Key2D,SumAgg2D>(-1, size, i, numAgents);
                addIndex(getJoinMethod(grid), i);
            }


            addWorkload(new TraceWorkload2D("traces/gauss1.dat", true), 20);
        }

        SimpleGridBucketSizeGauss() {}
        
    };

}

#endif /* _SIMPLE_GRID_BUCKET_SIZE_GAUSS_H_ */

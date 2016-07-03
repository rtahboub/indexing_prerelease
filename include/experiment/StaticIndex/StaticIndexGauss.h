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

#ifndef _STATIC_INDEX_GAUSS_H_
#define _STATIC_INDEX_GAUSS_H_

#include "2D/Experiment2D.h"
#include "2D/point_index/BinarySearch2D.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/CRTree.h"
#include "2D/point_index/SimpleGrid.h"
#include "2D/point_index/LinearizedKDTrie.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class StaticIndexGauss : public Experiment2D {
    public:
        void init() {

            folder = "out/traces/StaticIndex/StaticIndexGauss";
            experimentName = "StaticIndexGauss";
            addWorkloadParam("numHotSpots");
            

            int size = 22361;
            int numAgents = 50000;


            addIndex(getJoinMethod(new BinarySearch2D<Point2D, Key2D, SumAgg2D>()));
            addIndex(getJoinMethod(new RTree<Point2D, Key2D, SumAgg2D>()));
            addIndex(getJoinMethod(new CRTree<Point2D, Key2D, SumAgg2D>()));
            addIndex(getJoinMethod(new LinearizedKDTrie<Point2D, Key2D, SumAgg2D>()));

            //TODO: Change 16 to best bucket size. 
            addIndex(getJoinMethod(new SimpleGrid<Point2D, Key2D, SumAgg2D>(-1, size, 16, numAgents)));


            // addWorkload(new TraceWorkload2D("traces/gauss/gauss1trial1.dat", true), 20, 1);
            // addWorkload(new TraceWorkload2D("traces/gauss/gauss10trial1.dat", true), 20, 10);
            // addWorkload(new TraceWorkload2D("traces/gauss/gauss100trial1.dat", true), 20, 100);
            // addWorkload(new TraceWorkload2D("traces/gauss/gauss1000trial1.dat", true), 20, 1000);

            std::vector<Workload2D*>* workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1trial3.dat", true));
            addWorkload(workloads, "gauss1", 20, 1);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss10trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss10trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss10trial3.dat", true));
            addWorkload(workloads, "gauss10", 20, 10);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss100trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss100trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss100trial3.dat", true));
            addWorkload(workloads, "gauss100", 20, 100);

            workloads = new std::vector<Workload2D*>();
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1000trial1.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1000trial2.dat", true));
            workloads->push_back(new TraceWorkload2D("traces/gauss/gauss1000trial3.dat", true));
            addWorkload(workloads, "gauss1000", 20, 1000);

        }


        StaticIndexGauss() : Experiment2D() {}
    };
}

#endif /* _STATIC_INDEX_GAUSS_H_ */

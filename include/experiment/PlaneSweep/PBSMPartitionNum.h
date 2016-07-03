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

#ifndef _PBSM_PARTITION_NUM_H_
#define _PBSM_PARTITION_NUM_H_


#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/plane_sweep/ListSweep.h"
#include "2D/TestTypes2D.h"


namespace PSimIndex {

    /**
     *
     * Note: This tests ONLY the number of partitions, not the tile
     * dimension. We use uniform data, so there should be no skew anyway. 
     *
     */
    class PBSMPartitionNum : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/PlaneSweep/PBSMPartitionNum";
            experimentName = "PBSMPartitionNum";
            addIndexParam("NumPartitions");

            double* param;

            for(int i = 5; i < 150; i += 20) {

                SweepStructure<Point2D,Key2D>* sweepStruct = new ListSweep<Point2D,Key2D>();
                addIndex(
                    getJoinMethod(new ForwardPBSM2D<Point2D, Key2D, SumAgg2D, RegionGen2D>(i, i*i)),
                    i);
                
                // sweepStruct = new StripedSweep<Point2D,Key2D>(128);
                // addIndex(
                //     getJoinMethod(new GenericPBSM2D<Point2D, Key2D, SumAgg2D, RegionGen2D>(sweepStruct, i, i*i)),                    i);
                

            }



            std::vector<Workload2D*>* wc = new std::vector<Workload2D*>();
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial2.dat", true));
            wc->push_back(new TraceWorkload2D("traces/points/scalepoints50000trial3.dat", true));

            addWorkload(wc, "scalepoints50000", 0);

            // addWorkload(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true), 0);





            // addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0);
            // addWorkload(new TraceWorkload2D("traces/scalepoints90000.dat", true), 0);
            // addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0);
            // addWorkload(new TraceWorkload2D("traces/gauss1.dat", true), 20);

        }

        PBSMPartitionNum() : Experiment2D() {}
    };
}

#endif /* _PBSM_PARTITION_NUM_H_ */

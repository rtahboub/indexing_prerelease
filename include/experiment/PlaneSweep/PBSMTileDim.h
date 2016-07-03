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

#ifndef _PBSM_TILE_DIM_H_
#define _PBSM_TILE_DIM_H_


#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/plane_sweep/ListSweep.h"
#include "2D/TestTypes2D.h"


namespace PSimIndex {

    class PBSMTileDim : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/PlaneSweep/PBSMTileDim";
            experimentName = "PBSMTileDim";
            addIndexParam("TilesPerSide");

            double* param;
            for(int i = 45; i < 135; i += 20) {
                SweepStructure<Point2D,Key2D>* sweepStruct = new ListSweep<Point2D,Key2D>();
                addIndex(
                    getJoinMethod(new ForwardPBSM2D<Point2D, Key2D, SumAgg2D, RegionGen2D>(i, 45*45)),
                    i);
            }

            
            addWorkload(new TraceWorkload2D("traces/gauss/gauss1trial1.dat", true), 20, 1);
            addWorkload(new TraceWorkload2D("traces/gauss/gauss10trial1.dat", true), 20, 10);
            addWorkload(new TraceWorkload2D("traces/gauss/gauss100trial1.dat", true), 20, 100);
            addWorkload(new TraceWorkload2D("traces/gauss/gauss1000trial1.dat", true), 20, 1000);

        }

        PBSMTileDim() : Experiment2D() {}
    };
}

#endif /* _PBSM_TILE_DIM_H_ */

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

#ifndef _SYNCH_TRAVERSAL_NODE_SIZE_H_
#define _SYNCH_TRAVERSAL_NODE_SIZE_H_


#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/plane_sweep/ListSweep.h"
#include "2D/TestTypes2D.h"


namespace PSimIndex {

    class SynchTraversalNodeSize : public Experiment2D {
    public:
        void init() {
            folder = "out/traces/SynchTraversal/SynchTraversalNodeSize";
            experimentName = "SynchTraversalNodeSize";
            addIndexParam("NodeSize");

            double* param;

            for(int i = 8; i < 408; i += 100) {
                param = new double[i];
                param[0] = i;
                
                addIndex(
                    getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(STD,i)),
                    i);

                addIndex(
                    getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(RES,i)),
                    i);

                addIndex(
                    getJoinMethod(new SynchTraversal<Point2D, Key2D, SumAgg2D, RegionGen2D>(SWEEP,i)), 
                    i);
            }
	    

            addWorkload(new TraceWorkload2D("traces/points/scalepoints50000trial1.dat", true), 0);
            //addWorkload(new TraceWorkload2D("traces/scalepoints9000.dat", true), 0);
	    //addWorkload(new TraceWorkload2D("traces/scalepoints90000.dat", true), 0);
            //addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0);
            //addWorkload(new TraceWorkload2D("traces/gauss1.dat", true), 20);
            //addWorkload(new TraceWorkload2D("traces/scalepoints3000.dat", true));
        }

        SynchTraversalNodeSize() : Experiment2D() {}
    };
}

#endif /* _SYNCH_TRAVERSAL_NODE_SIZE_H_ */

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

 #include "experiment/StaticIndex/StaticIndexScalePoints.h"
 #include "experiment/StaticIndex/StaticIndexSimpleGrid10000.h"
 #include "experiment/StaticIndex/StaticIndexSimpleGrid30000.h"
 #include "experiment/StaticIndex/StaticIndexSimpleGrid50000.h"
 #include "experiment/StaticIndex/StaticIndexSimpleGrid70000.h"
 #include "experiment/StaticIndex/StaticIndexSimpleGrid90000.h"
 #include "experiment/StaticIndex/StaticIndexScaleQueries.h"
 #include "experiment/StaticIndex/StaticIndexScaleUpdates.h"
 #include "experiment/StaticIndex/StaticIndexGauss.h"
 #include "experiment/StaticIndex/StaticIndexHuge.h"

 #include "experiment/StaticJoin/StaticJoinScalePoints.h"
 #include "experiment/StaticJoin/StaticJoinScaleQueries.h"
 #include "experiment/StaticJoin/StaticJoinScaleUpdates.h"
 #include "experiment/StaticJoin/StaticJoinGauss.h"

 #include "experiment/MovingIndex/MovingIndexScalePoints.h"
 #include "experiment/MovingIndex/MovingIndexSTRIPES10000.h"
 #include "experiment/MovingIndex/MovingIndexSTRIPES30000.h"
 #include "experiment/MovingIndex/MovingIndexSTRIPES50000.h"
 #include "experiment/MovingIndex/MovingIndexSTRIPES70000.h"
 #include "experiment/MovingIndex/MovingIndexSTRIPES90000.h"
 #include "experiment/MovingIndex/MovingIndexScaleQueries.h"
 #include "experiment/MovingIndex/MovingIndexScaleUpdates.h"
 #include "experiment/MovingIndex/MovingIndexGauss.h"
 #include "experiment/MovingIndex/MovingIndexHorizon.h"
 #include "experiment/LargeTest/LargeTest.h"

 #include "experiment/RTree/RTreeNodeSize.h"
 #include "experiment/RTree/RTreeNodeSizeGauss.h"
 #include "experiment/RTree/CRTree8NodeSize.h"
 #include "experiment/RTree/CRTree16NodeSize.h"
 #include "experiment/RTree/CRTree32NodeSize.h"
 #include "experiment/RTree/FillFactor.h"

 #include "experiment/PlaneSweep/PBSMPartitionNum.h"
 #include "experiment/PlaneSweep/PBSMTileDim.h"

 #include "experiment/PlaneSweep/PlaneSweepScalePoints.h"
 #include "experiment/PlaneSweep/PlaneSweepScaleQueries.h"
 #include "experiment/PlaneSweep/PlaneSweepScaleUpdates.h"
 #include "experiment/PlaneSweep/PlaneSweepGauss.h"



 #include "experiment/SynchTraversal/SynchTraversalScalePoints.h"
 #include "experiment/SynchTraversal/SynchTraversalScaleQueries.h"
 #include "experiment/SynchTraversal/SynchTraversalScaleUpdates.h"
 #include "experiment/SynchTraversal/SynchTraversalNodeSize.h"
 #include "experiment/SynchTraversal/SynchTraversalGauss.h"

 #include "experiment/LinearizedKDTrie/TestKDTrieThreshold.h"
 #include "experiment/LinearizedKDTrie/TestKDTrieThresholdHuge.h"

#include "experiment/SimpleGrid/NumGridCellsSmall.h"
#include "experiment/SimpleGrid/NumGridCellsLarge.h"
#include "experiment/SimpleGrid/NumGridCellsHuge.h"
#include "experiment/SimpleGrid/NumGridCellsGauss.h"
#include "experiment/SimpleGrid/SimpleGridBucketSizeSmall.h"
#include "experiment/SimpleGrid/SimpleGridBucketSizeLarge.h"
#include "experiment/SimpleGrid/SimpleGridBucketSizeHuge.h"
#include "experiment/SimpleGrid/SimpleGridBucketSizeGauss.h"

#include "experiment/TPRTree/TestTPRHorizon.h"
#include "experiment/TPRTree/TPRTreeNodeSize.h"
#include "experiment/TPRTree/TPRTreeNodeMin.h"
#include "experiment/TPRTree/TestTPREntries.h"

#include "experiment/STRIPES/STRIPESHorizonSmall.h"
#include "experiment/STRIPES/STRIPESHorizonLarge.h"
#include "experiment/STRIPES/STRIPESHorizonHuge.h"
#include "experiment/STRIPES/STRIPESHorizonGauss.h"

#include "experiment/STRIPES/VXMax.h"

#include "experiment/Network/ScaleNetwork.h"

#include "experiment/ContJoin/ContJoinHorizon.h"
#include "experiment/ContJoin/ContJoinScalePoints.h"
#include "experiment/ContJoin/ContJoinScaleQueries.h"
#include "experiment/ContJoin/ContJoinScaleUpdates.h"
#include "experiment/ContJoin/ContJoinGauss.h"

namespace PSimIndex {

    Experiment2D* getExperimentByName(const char* name) {
        std::cout << "Experiment name " << name << std::endl;

        if(strcmp(name, "StaticIndexScalePoints") == 0) {
            return new StaticIndexScalePoints();
        }
        if(strcmp(name, "StaticIndexSimpleGrid10000") == 0) {
            return new StaticIndexSimpleGrid10000();
        }
        if(strcmp(name, "StaticIndexSimpleGrid30000") == 0) {
            return new StaticIndexSimpleGrid30000();
        }
        if(strcmp(name, "StaticIndexSimpleGrid50000") == 0) {
            return new StaticIndexSimpleGrid50000();
        }
        if(strcmp(name, "StaticIndexSimpleGrid70000") == 0) {
            return new StaticIndexSimpleGrid70000();
        }
        if(strcmp(name, "StaticIndexSimpleGrid90000") == 0) {
            return new StaticIndexSimpleGrid90000();
        }
        else if(strcmp(name, "StaticIndexScaleQueries") == 0) {
            return new StaticIndexScaleQueries();
        }
        else if(strcmp(name, "StaticIndexScaleUpdates") == 0) {
            return new StaticIndexScaleUpdates();
        }
        else if(strcmp(name, "StaticIndexGauss") == 0) {
            return new StaticIndexGauss();
        }
        else if(strcmp(name, "StaticIndexHuge") == 0) {
            return new StaticIndexHuge();
        }
        else if(strcmp(name, "StaticJoinScalePoints") == 0) {
            return new StaticJoinScalePoints();
        }
        else if(strcmp(name, "StaticJoinScaleQueries") == 0) {
            return new StaticJoinScaleQueries();
        }
        else if(strcmp(name, "StaticJoinScaleUpdates") == 0) {
            return new StaticJoinScaleUpdates();
        }
        else if(strcmp(name, "StaticJoinGauss") == 0) {
            return new StaticJoinGauss();
        }
        else if(strcmp(name, "MovingIndexScalePoints") == 0) {
            return new MovingIndexScalePoints();
        }
        else if(strcmp(name, "MovingIndexSTRIPES10000") == 0) {
            return new MovingIndexSTRIPES10000();
        }
        else if(strcmp(name, "MovingIndexSTRIPES30000") == 0) {
            return new MovingIndexSTRIPES30000();
        }
        else if(strcmp(name, "MovingIndexSTRIPES50000") == 0) {
            return new MovingIndexSTRIPES50000();
        }
        else if(strcmp(name, "MovingIndexSTRIPES70000") == 0) {
            return new MovingIndexSTRIPES70000();
        }
        else if(strcmp(name, "MovingIndexSTRIPES90000") == 0) {
            return new MovingIndexSTRIPES90000();
        }
        else if(strcmp(name, "MovingIndexScaleQueries") == 0) {
            return new MovingIndexScaleQueries();
        }
        else if(strcmp(name, "MovingIndexScaleUpdates") == 0) {
            return new MovingIndexScaleUpdates();
        }
        else if(strcmp(name, "MovingIndexGauss") == 0) {
            return new MovingIndexGauss();
        }
        else if(strcmp(name, "LargeTest") == 0) {
            return new LargeTest();
        }
        else if(strcmp(name, "PBSMPartitionNum") == 0) {
            return new PBSMPartitionNum();
        }
        else if(strcmp(name, "PBSMTileDim") == 0) {
            return new PBSMTileDim();
        }
        else if(strcmp(name, "RTreeNodeSize") == 0) {
            return new RTreeNodeSize();
        }
        else if(strcmp(name, "CRTree8NodeSize") == 0) {
            return new CRTree8NodeSize();
        }
        else if(strcmp(name, "CRTree16NodeSize") == 0) {
            return new CRTree16NodeSize();
        }
        else if(strcmp(name, "CRTree32NodeSize") == 0) {
            return new CRTree32NodeSize();
        }
        else if(strcmp(name, "FillFactor") == 0) {
            return new FillFactor();
        }
        else if(strcmp(name, "RTreeNodeSizeGauss") == 0) {
             return new RTreeNodeSizeGauss();
        }
        else if(strcmp(name, "TestKDTrieThreshold") == 0) {
            return new TestKDTrieThreshold();
        }
        else if(strcmp(name, "TestKDTrieThresholdHuge") == 0) {
            return new TestKDTrieThresholdHuge();
        }
        else if(strcmp(name, "MovingIndexHorizon") == 0) {
            return new MovingIndexHorizon();
        }
        else if(strcmp(name, "PlaneSweepScalePoints") == 0) {
            return new PlaneSweepScalePoints();
        }
        else if(strcmp(name, "PlaneSweepScaleQueries") == 0) {
            return new PlaneSweepScaleQueries();
        }
        else if(strcmp(name, "PlaneSweepScaleUpdates") == 0) {
            return new PlaneSweepScaleUpdates();
        }
        else if(strcmp(name, "SynchTraversalScalePoints") == 0) {
            return new SynchTraversalScalePoints();
        }
        else if(strcmp(name, "SynchTraversalScaleQueries") == 0) {
            return new SynchTraversalScaleQueries();
        }
        else if(strcmp(name, "SynchTraversalScaleUpdates") == 0) {
            return new SynchTraversalScaleUpdates();
        }
        else if(strcmp(name, "SynchTraversalGauss") == 0) {
            return new SynchTraversalGauss();
        }
        else if(strcmp(name, "SynchTraversalNodeSize") == 0) {
            return new SynchTraversalNodeSize();
        }
        else if(strcmp(name, "PlaneSweepGauss") == 0) {
            return new PlaneSweepGauss();
        }
        else if(strcmp(name, "TestTPRHorizon") == 0) {
            return new TestTPRHorizon();
        }
        else if(strcmp(name, "TPRTreeNodeSize") == 0) {
            return new TPRTreeNodeSize();
        }
        else if(strcmp(name, "TPRTreeNodeMin") == 0) {
            return new TPRTreeNodeMin();
        }
        else if(strcmp(name, "TestTPREntries") == 0) {
            return new TestTPREntries();
        }
        else if(strcmp(name, "STRIPESHorizonSmall") == 0) {
            return new STRIPESHorizonSmall();
        }
        else if(strcmp(name, "STRIPESHorizonLarge") == 0) {
            return new STRIPESHorizonLarge();
        }
        else if(strcmp(name, "STRIPESHorizonHuge") == 0) {
            return new STRIPESHorizonHuge();
        }
        else if(strcmp(name, "STRIPESHorizonGauss") == 0) {
            return new STRIPESHorizonGauss();
        }
        else if(strcmp(name, "NumGridCellsSmall") == 0) {
            return new NumGridCellsSmall();
        }
        else if(strcmp(name, "NumGridCellsLarge") == 0) {
            return new NumGridCellsLarge();
        }
        else if(strcmp(name, "NumGridCellsHuge") == 0) {
            return new NumGridCellsHuge();
        }
        else if(strcmp(name, "NumGridCellsGauss") == 0) {
            return new NumGridCellsGauss();
        }
        else if(strcmp(name, "SimpleGridBucketSizeSmall") == 0) {
            return new SimpleGridBucketSizeSmall();
        }
        else if(strcmp(name, "SimpleGridBucketSizeLarge") == 0) {
            return new SimpleGridBucketSizeLarge();
        }
        else if(strcmp(name, "SimpleGridBucketSizeHuge") == 0) {
            return new SimpleGridBucketSizeHuge();
        }
        else if(strcmp(name, "SimpleGridBucketSizeGauss") == 0) {
            return new SimpleGridBucketSizeGauss();
        }
        else if(strcmp(name, "ScaleNetwork") == 0) {
            return new ScaleNetwork();
        }
        else if(strcmp(name, "ContJoinHorizon") == 0) {
            return new ContJoinHorizon();
        }
        else if(strcmp(name, "ContJoinScalePoints") == 0) {
            return new ContJoinScalePoints();
        }
        else if(strcmp(name, "ContJoinScaleQueries") == 0) {
            return new ContJoinScaleQueries();
        }
        else if(strcmp(name, "ContJoinScaleUpdates") == 0) {
            return new ContJoinScaleUpdates();
        }
        else if(strcmp(name, "ContJoinGauss") == 0) {
            return new ContJoinGauss();
	}
        else if(strcmp(name, "VXMax") == 0) {
            return new VXMax();
        }
        else {
            // TODO: Come up with consistent error handling. 
            std::cerr << "Unknown experiment type";
            exit(1);
        }
    }
}

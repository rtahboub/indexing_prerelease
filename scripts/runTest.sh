#!/bin/bash

# ./bin/index.exec run --index=AEJoin --trace=traces/query_update/query0.1.update0.5conttrial1.dat
# ./bin/index.exec run --index=AEJoin --trace=traces/query_update/query0.1.update0.5conttrial2.dat
# ./bin/index.exec run --index=AEJoin --trace=traces/query_update/query0.1.update0.5conttrial3.dat

# ./bin/index.exec run --index=AEJoin --trace=traces/query_update/query0.3.update0.5conttrial1.dat
# ./bin/index.exec run --index=AEJoin --trace=traces/query_update/query0.3.update0.5conttrial2.dat
# ./bin/index.exec run --index=AEJoin --trace=traces/query_update/query0.3.update0.5conttrial3.dat

# ./bin/index.exec run --index=AEJoin --trace=traces/query_update/largequery0.0001.update0.001conttrial1.dat >> foo



# ./bin/index.exec run --index=RTree --trace=traces/query_update/largequery0.0001.update0.001trial1.dat >> foo
# ./bin/index.exec run --index=SynchTraversal2D --trace=traces/query_update/largequery0.0001.update0.001trial1.dat >> foo
# ./bin/index.exec run --index=TPRTree --trace=traces/query_update/largequery0.0001.update0.001trial1.dat >> foo
# ./bin/index.exec run --index=Stripes --trace=traces/query_update/largequery0.0001.update0.001trial1.dat >> foo


# ./bin/index.exec run --index=RTree --trace=traces/query_update/largequery0.001.update0.001trial1.dat >> foo
# ./bin/index.exec run --index=SynchTraversal2D --trace=traces/query_update/largequery0.001.update0.001trial1.dat >> foo
# ./bin/index.exec run --index=TPRTree --trace=traces/query_update/largequery0.001.update0.001trial1.dat >> foo
# ./bin/index.exec run --index=Stripes --trace=traces/query_update/largequery0.001.update0.001trial1.dat >> foo


# ./bin/index.exec run --index=RTree --trace=traces/query_update/largequery0.0001.update0.01trial1.dat >> foo
# ./bin/index.exec run --index=SynchTraversal2D --trace=traces/query_update/largequery0.0001.update0.01trial1.dat >> foo
# ./bin/index.exec run --index=TPRTree --trace=traces/query_update/largequery0.0001.update0.01trial1.dat >> foo
# ./bin/index.exec run --index=Stripes --trace=traces/query_update/largequery0.0001.update0.01trial1.dat >> foo


./bin/index.exec run --index=AEJoin --trace=traces/query_update/largequery0.0001.update0.001trial1cont.dat >> foo2
./bin/index.exec run --index=AEJoin --trace=traces/query_update/largequery0.001.update0.001trial1cont.dat >> foo2
./bin/index.exec run --index=AEJoin --trace=traces/query_update/largequery0.0001.update0.01trial1cont.dat >> foo2



# ./bin/index.exec experiment --name=MovingIndexGauss
# ./bin/index.exec experiment --name=MovingIndexScaleUpdates
# ./bin/index.exec experiment --name=MovingIndexScalePoints
# ./bin/index.exec experiment --name=MovingIndexSTRIPES10000
# ./bin/index.exec experiment --name=MovingIndexSTRIPES30000
# ./bin/index.exec experiment --name=MovingIndexSTRIPES50000
# ./bin/index.exec experiment --name=MovingIndexSTRIPES70000
# ./bin/index.exec experiment --name=MovingIndexSTRIPES90000


#./bin/index.exec experiment --name=PlaneSweepScaleQueries
# ./bin/index.exec experiment --name=PlaneSweepScaleUpdates




# ./bin/index.exec experiment --name=PlaneSweepScalePoints
# ./bin/index.exec experiment --name=PlaneSweepScaleQueries
# ./bin/index.exec experiment --name=PlaneSweepScaleUpdates

# ./bin/index.exec experiment --name=StaticIndexScalePoints
# ./bin/index.exec experiment --name=StaticIndexSimpleGrid10000
# ./bin/index.exec experiment --name=StaticIndexSimpleGrid30000
# ./bin/index.exec experiment --name=StaticIndexSimpleGrid50000
# ./bin/index.exec experiment --name=StaticIndexSimpleGrid70000
# ./bin/index.exec experiment --name=StaticIndexSimpleGrid90000

# ./bin/index.exec experiment --name=StaticIndexScaleQueries
# ./bin/index.exec experiment --name=StaticIndexScaleUpdates
# ./bin/index.exec experiment --name=StaticIndexGauss

# ./bin/index.exec experiment --name=StaticJoinScalePoints
# ./bin/index.exec experiment --name=StaticJoinScaleQueries
# ./bin/index.exec experiment --name=StaticJoinScaleUpdates
# ./bin/index.exec experiment --name=StaticJoinGauss

# ./bin/index.exec experiment --name=PBSMPartitionNum
# ./bin/index.exec experiment --name=NumGridCellsLarge
# ./bin/index.exec experiment --name=TPRTreeNodeSize
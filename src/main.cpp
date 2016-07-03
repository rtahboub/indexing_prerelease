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

 #include <iostream>
 #include <fstream>
 #include <vector>
 #include <cstring>
 #include <stdexcept>

 #include <sched.h>

 #include "util/Log.h"
 #include "IndexLog.h"
 #include "TimingResult.h"


 #include "2D/TestTypes2D.h"
 #include "2D/Workload2D.h"
 #include "2D/Experiment2D.h"
 #include "2D/JoinRunner2D.h"
 #include "2D/Region2D.h"

 // Workloads
 #include "2D/workload/TraceWorkload2D.h"

// Add new index includes here.
#include "2D/point_index/NaiveSpatialIndex2D.h"
#include "2D/point_index/NaiveMovingIndex2D.h"
#include "2D/point_index/BinarySearch2D.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/CRTree.h"

#include "2D/point_index/LinearizedKDTrie.h"
#include "2D/point_index/Stripes.h"
#include "2D/point_index/TPRTree.h"
#include "2D/point_index/SimpleGrid.h"

#include "2D/join_index/StaticNestedLoops2D.h"
#include "2D/join_index/MovingNestedLoops2D.h"
#include "2D/join_index/GenericPlaneSweep2D.h"
#include "2D/join_index/ForwardPlaneSweep.h"
#include "2D/join_index/ForwardPBSM2D.h"
#include "2D/join_index/GenericPBSM2D.h"
#include "2D/join_index/SynchTraversal.h"
#include "2D/join_index/plane_sweep/SweepStructure.h"

#include "2D/continuous_join/TrivialContinuousJoin.h"
#include "2D/continuous_join/AEJoin.h"

#include "experiment/experiments-inc.h"

using namespace PSimIndex;
using namespace DBUtl;

clock_t startbuild;
high_resolution_clock::time_point var;


namespace /* anon */ {

    void throwUsage(const char* msg) {
        throw std::logic_error(msg);
    }

    /**
     * TODO: Clean this up. It's really ugly and hard to read. 
     */
    JoinMethod* getJoinByName(const char* name) {

        if(strcmp(name,
                  NaiveSpatialIndex2D<Point2D,Key2D,SumAgg2D>::getClassName())
           == 0) {
            IndexType* index =
                new NaiveSpatialIndex2D<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                  NaiveMovingIndex2D<Point2D,Key2D,SumAgg2D>::getClassName())
           == 0) {
            IndexType* index =
                new NaiveMovingIndex2D<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                       BinarySearch2D<Point2D,Key2D,SumAgg2D>::getClassName())
                == 0) {
            IndexType* index =new BinarySearch2D<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                       RTree<Point2D,Key2D,SumAgg2D>::getClassName()) == 0) {
            IndexType* index =new RTree<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                       CRTree<Point2D,Key2D,SumAgg2D>::getClassName()) == 0) {
            IndexType* index =new CRTree<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                       Stripes<Point2D,Key2D,SumAgg2D>::getClassName()) == 0) {
            //TODO: replace maxInt by the parameter passed from datagen
            IndexType* index =new Stripes<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                       TPRTree<Point2D,Key2D,SumAgg2D>::getClassName()) == 0) {
            IndexType* index =new TPRTree<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                       SimpleGrid<Point2D,Key2D,SumAgg2D>::getClassName()) == 0) {
            IndexType* index =new SimpleGrid<Point2D, Key2D, SumAgg2D>();
            return getJoinMethod(index);
        }
        else if(strcmp(name,
                      LinearizedKDTrie<Point2D,Key2D,SumAgg2D>::getClassName()) == 0) {
           IndexType* index =new LinearizedKDTrie<Point2D, Key2D, SumAgg2D>();
           return getJoinMethod(index);
        }


        // Add a new else if for each new index or join. If you are adding a new
        // index, wrap the index instance by using the indexToJoin function. If
        // you are addint a new join method, return a new instance directly.

        else if(strcmp(name,
                       StaticNestedLoops2D<Point2D,
                                           Key2D,
                                           SumAgg2D,
                                           RegionGen2D>::getClassName()) == 0) {
            throwUsage("Cannot Initialize NestedLoops join directly.\n \
                        Use the name of the static index instead.");
        }
        else if(strcmp(name, MovingNestedLoops2D<Point2D,
                                                 Key2D,
                                                 SumAgg2D,
                                                 RegionGen2D>::getClassName())
                == 0) {
            throwUsage("Cannot Initialize NestedLoops join directly.\n \
                        Use the name of the static index instead.");
        }
        else if(strcmp(name, GenericPlaneSweep2D<Point2D,
                                          Key2D,
                                          SumAgg2D,
                                          RegionGen2D>::getClassName()) == 0) {
            return getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>());
        }
        else if(strcmp(name, "ListSweep2D") == 0) {
            SweepStructure<Point2D,Key2D>* ss = new ListSweep<Point2D,Key2D>();
            return getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(ss));
        }
        else if(strcmp(name, "StripedSweep2D") == 0) {
            SweepStructure<Point2D,Key2D>* ss = new StripedSweep<Point2D,Key2D>(32);
            return getJoinMethod(new GenericPlaneSweep2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(ss));
        }
        else if(strcmp(name, ForwardPlaneSweep<Point2D,
                                          Key2D,
                                          SumAgg2D,
                                          RegionGen2D>::getClassName()) == 0) {
            return getJoinMethod(new ForwardPlaneSweep<Point2D,Key2D,SumAgg2D,RegionGen2D>());
        }
        else if(strcmp(name, ForwardPBSM2D<Point2D,
                                          Key2D,
                                          SumAgg2D,
                                          RegionGen2D>::getClassName()) == 0) {
            return getJoinMethod(new ForwardPBSM2D<Point2D,Key2D,SumAgg2D,RegionGen2D>());
        }

        else if(strcmp(name, GenericPBSM2D<Point2D,
                                          Key2D,
                                          SumAgg2D,
                                          RegionGen2D>::getClassName()) == 0) {
            return getJoinMethod(new GenericPBSM2D<Point2D,Key2D,SumAgg2D,RegionGen2D>());
        }
        else if(strcmp(name, SynchTraversal<Point2D,
                                          Key2D,
                                          SumAgg2D,
                                          RegionGen2D>::getClassName()) == 0) {
            return getJoinMethod(new SynchTraversal<Point2D,Key2D,SumAgg2D,RegionGen2D>(STD,108));
        }
        else if(strcmp(name, "SynchTraversal2DRes") == 0) {
            return getJoinMethod(new SynchTraversal<Point2D,Key2D,SumAgg2D,RegionGen2D>(RES));
        }
        else if(strcmp(name, "SynchTraversal2DSweep") == 0) {
            return getJoinMethod(new SynchTraversal<Point2D,Key2D,SumAgg2D,RegionGen2D>(SWEEP));
        }
	else if(strcmp(name, TrivialContinuousJoin<Point2D,
                                    Key2D,
                                    SumAgg2D,
                                    RegionGen2D>::getClassName())
                        == 0) {
            ContJoinType *j;
            j = new TrivialContinuousJoin<Point2D,Key2D,SumAgg2D,RegionGen2D>();
            return getJoinMethod(j);
        }
	else if(strcmp(name, AEJoin<Point2D,
                                      Key2D,
                                      SumAgg2D,
                                      RegionGen2D>::getClassName())
                        == 0) {
            ContJoinType *j;
            j = new AEJoin<Point2D,Key2D,SumAgg2D,RegionGen2D>();
            return getJoinMethod(j);
        }
        else {
            throwUsage("Unknown join or index name");
        }
    }

    Workload2D* getWorkload(const char* traceFile) {
        bool binary;

        if(strstr(traceFile, ".dat") != 0) {
            binary = true;
        }
        else if(strstr(traceFile,".txt") != 0) {
            binary = false;
        }
        else {
            throwUsage("Unknown workload file extension. \
                        Should be .dat or .txt");
        }

        LOG(DEBUG) << "Parsing file " << traceFile << " as a "
                   << (binary ? "binary " : "text ") << "file" << std::endl;

        return new TraceWorkload2D(traceFile, binary);
    }


    // Taken from Al's code. Should use DBUtl/CmdLine, but the logging is
    // wrong...
    char const * suffixAfterMatch(const char *s, const char *flagArg) {
        size_t n = strlen(flagArg);
        char const * ans = ((strncmp(s, flagArg, n) == 0) ? (s+n) : 0);
        return ans;
    }


    enum RunMode { TEST, EXPER, RUN, TEST_TRACE };


    void compareTrace(JoinMethod* i1, JoinMethod* i2, const char* tracefile) {
        Workload2D* workload = getWorkload(tracefile);
        Workload2D* workload2 = getWorkload(tracefile);
        try {

            LOG(INFO) << "Testing " << i1->getName() << " and "
                      << i2->getName() << " on trace "
                      << tracefile << std::endl;

            bool ans = JoinRunner2D::compareIndices(i1, i2, workload, workload2);

            if(ans) {
                LOG(INFO) << "PASSED test on trace " << tracefile << std::endl;
            }
            else {
                LOG(INFO) << "FAILED test on trace " << tracefile << std::endl;
            }

        }
        catch(std::logic_error e) {
            LOG(ERR) << e.what() << std::endl;
        }

        delete workload;
        delete workload2;
    }
}



int main(int argc, char *argv[]) {


#ifndef __APPLE__
    // Set affinity of process to one cpu. 
    cpu_set_t  mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    int result = sched_setaffinity(0, sizeof(mask), &mask);
    if(result == -1) {
        throwUsage("Error setting process affinity.");
    }
#endif

    IndexLog::setLevel(INFO);

    Log::open("-"); //for asserts;
//    Log::setLevel(Log::DEBUG);

    JoinType* j;

    //assert
    RunMode mode;

    if(argc < 2) {
        throwUsage("Too few arguments");
    }

    const char* modeString = argv[1];

    // Trace mode variables
    JoinMethod* index1 = 0;
    JoinMethod* index2 = 0;
    Workload2D* workload = 0;
    std::ostream* traceStream = 0;

    // Experiment mode variables
    Experiment2D* exper = 0;
    int numTrials = 1;

    const char* workloadName;
    const char* index1Name;
    const char* index2Name;

    bool setWorkload = false;



    if(strcmp(modeString, "test") == 0) {
        mode = TEST;
    }
    else if(strcmp(modeString, "experiment") == 0) {
        mode = EXPER;
    }
    else if(strcmp(modeString, "run") == 0) {
        mode = RUN;
    }
    else if(strcmp(modeString, "test_trace") == 0) {
        mode = TEST_TRACE;
    }


    for(int argNum = 2; argNum < argc; argNum++) {
        char const * arg = argv[argNum];
        char const * v;

        if( ((v = suffixAfterMatch(arg, "--index=")) != 0)
            || ((v = suffixAfterMatch(arg, "--index1=")) != 0)) {

            if(mode == EXPER || mode == TEST_TRACE ) {
                throwUsage("--index only valid in test and run mode.");
            }
            index1 = getJoinByName(v);
            index1Name = v;
        }
        else if((v = suffixAfterMatch(arg, "--index2=")) != 0) {
            if(mode != TEST) {
                throwUsage("--index2 only valid in test mode.");
            }

            index2 = getJoinByName(v);
        }
        else if((v = suffixAfterMatch(arg, "--trace=")) != 0) {
            if(mode == EXPER) {
                throwUsage("--trace only valid in test, test_trace, or run  mode.");
            }

            workloadName = v;
            setWorkload = true;
            //= getJoinByName(v);
        }
        else if((v = suffixAfterMatch(arg, "--name=")) != 0) {
            if(mode != EXPER) {
                throwUsage("--name only valid in experiment mode.");
            }



            exper = getExperimentByName(v);

        }
        else if((v = suffixAfterMatch(arg, "--numTrials=")) != 0) {
            if(mode != EXPER) {
                throwUsage("--numTrials only valid in experiment mode.");
            }
            throwUsage("Unsupported");
        }

        else if((v = suffixAfterMatch(arg, "--log=")) != 0) {
            IndexLog::setLevel(IndexLog::strToLevel(v));
        }
        else if((v = suffixAfterMatch(arg, "--log-file=")) != 0) {
            if(traceStream == 0) {
                traceStream = new std::ofstream(v);
            }
            IndexLog::setStream(*traceStream);
        }
    }

    if(mode == TEST) {
        if(index1 == 0) {
            throwUsage("Must specify at least one index in test mode.");
        }
        if(index2 == 0) {
            index2 = getJoinByName("NaiveSpatialIndex2D");
        }
        
        try {
            if(setWorkload) {
                compareTrace(index1, index2, workloadName);
            }
            else {

                compareTrace(index1, index2, "traces/small_trace.txt");
                compareTrace(index1, index2, "traces/small_trace2.txt");
                compareTrace(index1, index2, "traces/small_trace3.txt");
                compareTrace(index1, index2, "traces/empty_trace.txt");
                compareTrace(index1, index2, "traces/duplicates_trace.txt");
                compareTrace(index1, index2, "traces/striped_test2.dat");
                compareTrace(index1, index2, "traces/random_small.txt");
                compareTrace(index1, index2, "traces/aealldelete.txt");
            }

            
            delete index1;
            delete index2;


        }
        catch(const std::exception& e) {
            std::cerr << e.what() <<std::endl;
        }

//        workload = getWorkload(workloadName);

//        Experiment2D::compareIndices(index1, index2, workload);
    }
     else if(mode == EXPER) {
         DBUTL_ASSERT((exper != 0));

         exper->init();
         exper->run();
         exper->close();
         
         delete exper;
     }
    else if(mode == RUN) {

        if(index1 == 0) {
            throwUsage("Must specify one index in run mode.");
        }
        if(!setWorkload) {
            throwUsage("Must specify a trace in run mode.");
        }

        startbuild = clock();
        var = high_resolution_clock::now();
        Workload2D* workload = getWorkload(workloadName);


        TimingResult* t = new TimingResult();


        JoinRunner2D::runJoinOnWorkload(index1, workload, t);


        std::cout << "t->getNumTrials() = " << t->getNumTrials() <<std::endl;
        std::cout << "t->getNumTicks() = " << t->getNumTicks() <<std::endl;

        std::cout << "Index Name: \t" << index1Name << std::endl;
        std::cout << "Workload Name: \t" << workload->getName() << std::endl;
        std::cout << "(Avg.) Total Time: \t" << t->getAvgTrialTime() << std::endl;
        std::cout << "(Avg.) Total Build Time: \t" << t->getAvgTrialTime(BUILD) << std::endl;
        std::cout << "(Avg.) Total Query Time: \t" << t->getAvgTrialTime(QUERY) << std::endl;
        std::cout << "(Avg.) Total Update Time: \t" << t->getAvgTrialTime(UPDATE) << std::endl;
        std::cout << "(Avg.) Total Num Results: \t" << t->getAvgTrialNumResults() << std::endl;

        // std::cout << std::endl;
        // for(int i = 0; i < t->getNumTicks(); i++) {

        //     for(int j = 0; j < t->getNumTrials(); j++) {
        //         printf("[%lf %lf %lf] %ld ", 
        //                t->getTime(j, i, BUILD),
        //                t->getTime(j, i, QUERY),
        //                t->getTime(j, i, UPDATE),
        //                t->getNumResults(j, i));
        //     }

        //     printf("\n");
        // }

        std::cout << std::endl;

        delete t;
        delete index1;
        delete workload;
    }
    else if(mode == TEST_TRACE) {
        throwUsage("FIXME");

        // if(!setWorkload) {
        //     throwUsage("Must specify a trace in test_trace mode.");
        // }

        // Workload2D* workload = getWorkload(workloadName);
        // Experiment2D::checkWorkload(workload);
    }

    return 0;
}

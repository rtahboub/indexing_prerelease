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

#ifndef _JOIN_RUNNER_2D_H_
#define _JOIN_RUNNER_2D_H_

#include "2D/TestTypes2D.h"
#include "TimingResult.h"
#include "2D/Workload2D.h"
#include "2D/IndexUtils2D.h"

#include "StaticJoinRunner2D.h"
#include "IncrementalJoinRunner2D.h"

namespace PSimIndex {
    

    /**
     * TODO: Add support for multiple queries for continuous queries.
     * TODO: Is the JoinMethod union still necessary?
     */    
    class JoinRunner2D {
    private:



        // /**
        //  * Compares two JoinCallBack objects. Logs a message at the ERR level
        //  * if any of the query results don't match.
        //  */
        // template<typename A, typename B>
        // static bool compareCallBacks(JoinCallBack<A,B>* cb1, 
        //                              JoinCallBack<A,B>* cb2,
        //                              int tick);


        // template<typename K, typename A, typename B>
        // static bool compareCallBacks(JoinCallBack<A,B>* cb1,
        //                              IncrementalCallBack<K,A,B>* cb2,
        //                              int tick);


        // // TODO: This is implemented by calling the previous version. This
        // // could be confusing as it will reverse the order of reported errors.
        // template<typename K, typename A, typename B>
        // static bool compareCallBacks(IncrementalCallBack<K,A,B>* cb2,
        //                              JoinCallBack<A,B>* cb1,                                      
        //                              int tick);


        // template<typename K, typename A, typename B>
        // static bool compareCallBacks(IncrementalCallBack<K,A,B>* cb1, 
        //                              IncrementalCallBack<K,A,B>* cb2,
        //                              int tick);


        // template<typename K, typename A, typename B>
        // static bool compareCallBacks(std::vector<CallBackMethod<K,A,B>*>* cbs1,
        //                              std::vector<CallBackMethod<K,A,B>*>* cbs2,
        //                              int tick);



        // static void updateContinuous(ContJoinType* index, 
        //                              Workload2D* pw,
        //                              std::tr1::unordered_set<Point2D*>* pointSet,
        //                              std::tr1::unordered_set<Point2D*>* querySet);

        // static std::vector<Point2D*>* getContQuery(Workload2D* pw, 
        //                                            RegionGen2D*& rg,
        //                                            std::vector<Point2D*>* pts);


        static AbstractJoinRunner2D* getRunner(JoinMethod* j, 
                                               Workload2D* pw, 
                                               bool copyPts, 
                                               TimingResult* tr);

    public:
        


        static bool compareIndices(JoinMethod* j1, 
                                   JoinMethod* j2, 
                                   Workload2D* pw1,
                                   Workload2D* pw2);


        //TODO: Support multiple trials.
        static void runJoinOnWorkload(JoinMethod* join,
                                      Workload2D* pw,
                                      TimingResult* tr);

    };

    // ----------------------------------------------------------------
    // Template Specialization Prototypes
    // ----------------------------------------------------------------

    // template<>
    // bool JoinRunner2D::ltPair<Point2D, Point2D>(
    //     std::pair<Point2D*,Point2D*>* o1,
    //     std::pair<Point2D*,Point2D*>* o2
    //     );

    // template<>
    // bool JoinRunner2D::ltPair<Point2D, SumAgg2D>(
    //     std::pair<Point2D*,SumAgg2D*>* o1,
    //     std::pair<Point2D*,SumAgg2D*>* o2
    //     );
}

#endif /* _JOIN_RUNNER_2D_H_ */

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

#ifndef _INCREMENTAL_JOINRUNNER_2D_H_
#define _INCREMENTAL_JOINRUNNER_2D_H_

#include "2D/TestTypes2D.h"
#include "2D/AbstractJoinRunner2D.h"
#include "TimingResult.h"
#include "2D/Workload2D.h"

namespace PSimIndex {

    /**
     * NOTE: Currently, we only support a single continuous query (which can
     * contain many points). Adding additional queries is possible, but it
     * would require changing the ContinuousJoin API to do parameterize
     * updateJoinResult and get/setRegionGen by the query number. This may or
     * may not be worth doing in the future depending on the performance we
     * see. 
     */ 
    class IncrementalJoinRunner2D : public AbstractJoinRunner2D {

    private:
       // typedef std::tr1::unordered_set<Point2D*> HashSet;
    	typedef std::unordered_set<Point2D*> HashSet;
        
        ContJoinType* join;
        Workload2D* workload;
        TimingResult* tr;
        int tick;
        bool copyPts;
        HashSet pointSet;
        HashSet querySet;        
        bool hasNext;
//        std::vector<HashSet*> querySet;

        std::vector<Point2D*>* qp;
        std::vector<Point2D*>* points;

        //std::vector<JoinCallBack<Key2D, Point2D, Point2D>*> cbs;

        void updateContinuous();

    public:

        bool hasNextTick();
        void runTick(JoinCallBack<Key2D,Point2D,Point2D>* cb);
        //std::vector<JoinCallBack<Key2D,Point2D,Point2D>*>* getCallBacks();
        //void clearCallBacks();

        TimingResult* getTimingResult();


        IncrementalJoinRunner2D(ContJoinType* join,
                                Workload2D* workload,
                                bool copyPts,
                                TimingResult* tr);

        ~IncrementalJoinRunner2D();

    };



}


#endif /* _INCREMENTAL_JOINRUNNER_2D_H_ */

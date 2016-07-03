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

#ifndef _STATIC_JOIN_RUNNER_2D_H_
#define _STATIC_JOIN_RUNNER_2D_H_

#include "2D/TestTypes2D.h"
#include "2D/AbstractJoinRunner2D.h"
#include "2D/Workload2D.h"
#include "TimingResult.h"


namespace PSimIndex {

    class StaticJoinRunner2D : public AbstractJoinRunner2D {

    private:

        JoinType* join;
        Workload2D* workload;
        TimingResult* tr;
        int tick;
        bool copyPts;

        //std::vector<JoinCallBack<Key2D,Point2D, Point2D>*> cbs;

//        JoinCallBack


        double processBuild(std::vector<Point2D*>* points);
        double processQueries(std::vector<Point2D*>* points, JoinCallBack<Key2D,Point2D,Point2D>* cb); 
        double processUpdates(bool force);


    public:

        
        bool hasNextTick();
        void runTick(JoinCallBack<Key2D,Point2D,Point2D>* cb);

        // std::vector<JoinCallBack<Key2D,Point2D,Point2D>*>* getCallBacks();
        // void clearCallBacks();

        
        TimingResult* getTimingResult();

        //void init();


        /**
         * The caller must pass in a non-NULL TimingResult. This simplifies
         * memory management and avoids unexpected deletes. 
         */
        StaticJoinRunner2D(JoinType* join, 
                           Workload2D* workload, 
                           bool copyPts, 
                           TimingResult* tr);



        ~StaticJoinRunner2D();
    };
}




#endif /* _STATIC_JOIN_RUNNER_2D_H_ */

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

#include "2D/JoinRunner2D.h"
#include "2D/IndexUtils2D.h"
#include "IndexLog.h"
#include "util/ptimer.h"
#include "util/Log.h"

#include "VectorGroupCallBack.h"
#include "IncrementalCallBack.h"

#include <stdexcept>

using namespace PSimIndex;
using namespace DBUtl;

AbstractJoinRunner2D* JoinRunner2D::getRunner(JoinMethod* j,
                                              Workload2D* pw, 
                                              bool copyPts, 
                                              TimingResult* tr) {
    //AbstractJoinRunner2D* jr;
    
    if(!j->isCont) {
        return new StaticJoinRunner2D(j->join, pw, copyPts, tr);
    }
    else {
        return new IncrementalJoinRunner2D(j->contJoin, pw, copyPts, tr);
    }

}



void JoinRunner2D::runJoinOnWorkload(JoinMethod* j1, 
                                     Workload2D* pw,
                                     TimingResult* tr) {

    AbstractJoinRunner2D* runner = getRunner(j1, pw, false, tr);

    JoinCallBack<Key2D, Point2D, Point2D>* cb;

    if(j1->isCont) {
        cb = new IncrementalCallBack<Key2D,Point2D,Point2D>();
    }
    else {
        //cb = new SetJoinCallBack<Key2D,Point2D,Point2D>();
        //cb = new StaticJoinCallBack<Key2D,Point2D,Point2D>();
        cb = new VectorGroupCallBack<Key2D,Point2D,Point2D>();
        //cb = new VectorCallBack<Key2D,Point2D,Point2D>();
        //cb = new IncrementalCallBack<Key2D,Point2D,Point2D>();
    }
    cb->setCopyPoints(false);

    int tickNum = 0;

    while(runner->hasNextTick()) {
//    for(int i = 0; i < 1; i++) {

      //std::cout << "tick " << tickNum << " ";
        runner->runTick(cb);

        //std::cout << "ending tick" << std::endl;
        // cb->printSorted();
        // std::cout << "--------------" << std::endl;
        if(!j1->isCont) {
            //std::cout << "HERE" << std::endl;
            cb->clear();
        }
	tickNum++;
    }


    delete cb;
    //runner->clearCallBacks();
    delete runner;
}

/**
 * TODO: What to do about cbs for cont join. We can't clear them at the end of
 * the tick. 
 */
bool JoinRunner2D::compareIndices(JoinMethod* j1, 
                                  JoinMethod* j2,
                                  Workload2D* pw1,
                                  Workload2D* pw2) {

    LOG(DEBUG) << "JoinRunner2D: Comparing " << j1->getName()
               << " and " << j2->getName()
               << " on " << pw1->getName() << std::endl;
    
    bool ans = true;
    int tick = 0;

    AbstractJoinRunner2D* jr1 = getRunner(j1, pw1, true, NULL);
    AbstractJoinRunner2D* jr2 = getRunner(j2, pw2, true, NULL);


    //std::vector<JoinCallBack<Key2D,Point2D,Point2D>*> *cbs1, *cbs2;

    IncrementalCallBack<Key2D,Point2D,Point2D> cb1;
    cb1.setCopyPoints(true);
    IncrementalCallBack<Key2D,Point2D,Point2D> cb2;
    cb2.setCopyPoints(true);

    // std::vector<JoinCallBack<Key2D,Point2D,Point2D>*>::iterator it;

    int tickNum = 0;
    while(jr1->hasNextTick()) {
        DBUTL_ASSERT((jr2->hasNextTick()));
        jr1->runTick(&cb1);
        jr2->runTick(&cb2);



        // std::cout << tickNum << ": " << cb1.numPairs() << " " << cb2.numPairs() << std::endl;


        // cbs1 = jr1->getCallBacks();
        // cbs2 = jr2->getCallBacks();


        // std::cout << "printing icb 2." << std::endl;
        // cbs1->at(0)->icb->printSorted(ltPair<Point2D,Point2D>);
        // std::cout << std::endl;


        //bool comp = compareCallBacks(cbs1, cbs2, tick++);
        bool comp = true;
        // //for(int i = 0; i < cbs1->size(); i++) {
        //     comp = (comp && (cbs1->at(i))->equals(cbs2->at(i)));
        // }

        comp = cb1.equals(&cb2);


        if(!comp) {
            std::cout << "ERROR: RESULTS DON'T MATCH at tick " << tickNum << std::endl;



            // std::cout << "printing icb ..." << std::endl;
            // cb1.printSorted();

            // std::cout << "printing icb 2..." << std::endl;
            // cb2.printSorted();

            exit(1);

        }
        ans = (ans && comp);


        if(!j1->isCont) {
            cb1.clear();
        }
        if(!j2->isCont) {
            cb2.clear();
        }

        tickNum++;
    }
    delete jr1;
    delete jr2;

    return ans;
}




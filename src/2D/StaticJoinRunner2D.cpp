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

#include "2D/StaticJoinRunner2D.h"
//#include "StaticJoinCallBack.h"

#include "2D/IndexUtils2D.h"
#include "IndexLog.h"
#include "util/ptimer.h"
#include "util/Log.h"

#include <stdexcept>


#if defined (VTUNE_BUILD) || defined (VTUNE_QUERY) || defined (VTUNE_UPDATE) || defined (VTUNE_ALL)
#include "VtuneApi.h"
#endif



using namespace PSimIndex;
using namespace DBUtl;


double StaticJoinRunner2D::processBuild(std::vector<Point2D*>* points) {
    PTimer pt;

    pt.start();

#ifdef VTUNE_BUILD
    VTResume();
#endif

    
    join->startTick(points);


#ifdef VTUNE_BUILD
    VTPause();
#endif

    pt.finish();

    return pt.realResult();
}


double StaticJoinRunner2D::processQueries(std::vector<Point2D*>* points, 
                                          JoinCallBack<Key2D,Point2D,Point2D>* cb) {
    PTimer pt;

    std::vector<Point2D*>* qp;

    //JoinCallBack<Key2D,Point2D,Point2D>* cb;

    int numPairs = 0;
    int qNum = 0;

    pt.start();

#ifdef VTUNE_QUERY
    VTResume();
#endif


    //while(workload->hasNextQuery()) {

        // if(cbs.size() <= qNum) {
        //     //cb = new StaticJoinCallBack<Key2D,Point2D,Point2D>(copyPts);
        //     //cb = new StaticJoinCallBackMap<Key2D,Point2D,Point2D>(copyPts);
        //     cb = new VectorGroupCallBack<Key2D,Point2D,Point2D>(copyPts);
        //     //cb = new DummyCallBack<Key2D,Point2D,Point2D>(copyPts);
        //     cbs.push_back(cb);
        // }
        // else {
        //     //DBUTL_ASSERT((!cbs[qNum]->isCont));
        //     cb = cbs[qNum];//->jcb;
        //     //DBUTL_ASSERT((cb->size() == 0));
        // }
    if(workload->hasNextQuery()) {
        QueryRec* query = workload->nextQuery();

        //Optimization for full self joins.
        qp = (query->queryPoints == 0) ? points : query->queryPoints;
        
        switch(query->type) {

        case ENUM:
            join->enumerationJoin(cb, qp, query->regionGen);
            // cb->sort(ltPair<Point2D,Point2D>);
            // cb->print();

            break;
            


        case AGG:
            throw std::logic_error("AGG queries not yet supported");
            break;

        case KNN:
            throw std::logic_error("KNN queries not yet supported");
            break;
        }
                
        qNum++;
    }
    if(workload->hasNextQuery()) {
        throw std::logic_error("Multiple queries unsupported in StaticJoinRunner2D.");
    }



#ifdef VTUNE_QUERY
    VTPause();
#endif

    pt.finish();

    // for(int i = 0; i < (cbs.size() - qNum); i++) {
    //     JoinCallBack<Key2D,Point2D,Point2D>* lastCB = cbs.back();
    //     //CallBackMethod<Key2D,Point2D,Point2D>* lastCB = cbs.back();
    //     cbs.pop_back();
    //     delete lastCB;
    // }

    return pt.realResult();
}


double StaticJoinRunner2D::processUpdates(bool force) {
    PTimer pt;
    MovingJoinType* mv;
    bool isMoving = false;

    if(join->isMovingJoin()) {
        isMoving = true;
        mv = (MovingJoinType*) join;
    }

    pt.start();

#ifdef VTUNE_UPDATE
    VTResume();
#endif
    while(workload->hasNextUpdate()) {

        UpdateRec* update = workload->nextUpdate();

        LOG(DEBUG) << "Experiment2D: Processing Update "
                   << *update << std::endl;

        Point2D* point = update->point;
        
        switch(update->type) {

        case VEL_UPDATE:

            if(isMoving) {
                mv->beforeUpdateVelocity(point, point->x, point->y,
                                         update->newVx, update->newVy);
            }
            point->vx = update->newVx;
            point->vy = update->newVy;

            // if(point->id == 27249) {
            //     std::cout << tick << " vel update " << *point << std::endl;
            // }

            break;

        case AGG_UPDATE:
            point->data = update->newAgg;
            join->afterUpdateAgg(point);
            break;

        case POS_UPDATE:

            join->beforeDelete(point);
            point->x = update->newX;
            point->y = update->newY;
            point->vx = update->newVx;
            point->vy = update->newVy;


            join->afterInsert(point);

            // if(point->id == 27249) {
            //     std::cout << tick << " pos update " << *point << std::endl;
            // }

            break;

        case INSERT:
            workload->insertPoint(point);
            join->afterInsert(point);

            // if(point->id == 27249) {
            //     std::cout << tick << " insert " << *point << std::endl;
            // }

            break;

        case DELETE:
            join->beforeDelete(point);
            workload->deletePoint(point);
            break;

        // an IMPLICIT_MOVE update updates the position of a point according
        // to its velocity. Moving object indices do not need to be informed
        // of these updates. Static indices will treat them like position
        // updates. 
        case IMPLICIT_MOVE:
            if(!isMoving) { join->beforeDelete(point); }
            point->x = point->x + point->vx;
            point->y = point->y + point->vy;

            //if(point->x < 0 || point->y < 0) {
	      //std::cout << "NEGATIVE POINTS" << std::endl;
	    //}


            if(!isMoving) { join->afterInsert(point); }

            // if(point->id == 27249) {
            //     std::cout << tick << " implicit move "<< *point << std::endl;
            // }
            

            break;
        }
    }
    join->endTick(force);

#ifdef VTUNE_UPDATE
    VTPause();
#endif

    pt.finish();

    return pt.realResult();
}

// std::vector<JoinCallBack<Key2D,Point2D,Point2D>*>* StaticJoinRunner2D::getCallBacks() {
//     if(!copyPts) {
//         std::cout << "throwing in getCallBacks()" << std::endl;
//         throw std::logic_error("copyPts must be set in StaticJoinRunner2D in order to call getCallbacks");
//     }
//     return &cbs;
// }

// void StaticJoinRunner2D::clearCallBacks() {
//     for(int i = 0; i < cbs.size(); i++) {
//         cbs[i]->clear();
//     }
// }

TimingResult* StaticJoinRunner2D::getTimingResult() {
    return tr;
}

bool StaticJoinRunner2D::hasNextTick() {
    return workload->hasNextTick();
}

void StaticJoinRunner2D::runTick(JoinCallBack<Key2D,Point2D,Point2D>* cb) {

    // TODO: How to fail gracefully if no more ticks?
    DBUTL_ASSERT((workload->hasNextTick()));
    //clearCallBacks();
    
//    if(this->copyPts != copyPts) {
    //cbs.clear();
//    }

    workload->startTick();

    std::vector<Point2D*>* points = workload->getPoints();


#ifdef VTUNE_ALL
    VTResume();
#endif

    double build = processBuild(points);
  //  clock_t finishbuild = clock();
   // std::cout << "build time ";
    //std::cout.precision(17);
   // std::cout << (finishbuild - startbuild)/ CLOCKS_PER_SEC;
//	std::cout		<< " \n";
    auto finishbuild = std::chrono::high_resolution_clock::now();
    std::cout << "build time " << std::chrono::duration_cast<std::chrono::milliseconds>(finishbuild-var).count()<< " milliseconds\n";

    auto startquery = std::chrono::high_resolution_clock::now();
//    clock_t startquery = clock();
    double query = processQueries(points, cb);
 //   clock_t finishquery = clock();
//    std::cout << "query time " << (finishquery - startquery)/ CLOCKS_PER_SEC << " \n";

    auto finishquery = std::chrono::high_resolution_clock::now();
    std::cout << "query time " << std::chrono::duration_cast<std::chrono::milliseconds>(finishquery-startquery).count()<< " milliseconds\n";

    int numResults = cb->numPairs();

    //int numResults = 0;

    // for(int i = 0; i < cbs.size(); i++) {
    //     numResults += cbs[i]->numPairs();
    // }

    //compute number of results before any updates. 

   // double update = processUpdates((!workload->hasNextTick()));
    double update = 0;

#ifdef VTUNE_ALL
    VTPause();
#endif

    // std::cout << "Printing static cb..." << std::endl;
    // cbs[0]->jcb->sort(ltPair<Point2D,Point2D>);
    // cbs[0]->jcb->print();
    // std::cout << std::endl;


    workload->endTick();
    delete points;

    //std::cout << "[ " << build << " " << query << " " << update << " ] " << numResults << std::endl;

    if(tr != NULL) {
        tr->addTick(build, query, update, numResults);
    }
    tick++;
    
}

StaticJoinRunner2D::StaticJoinRunner2D(JoinType* join, 
                                       Workload2D* workload, 
                                       bool copyPts,
                                       TimingResult* tr) {
    this->join = join;
    this->workload = workload;
    tick = 0; 
    this->copyPts = copyPts;


    // if(tr == NULL) {
    //     throw std::logic_error("In StaticJoinRunner2D constructor. \ 
    //                             TimingResult tr must not be null");        
    // }
    this->tr = tr;
}

StaticJoinRunner2D::~StaticJoinRunner2D() {
    //Need to be very careful about this, since it may invalidate TimingResult
    //object used by caller. 
    //std::cout << "In StaticJoinRunner2D destructor cbs.size() = " << cbs.size() << std::endl;
    // for(int i = 0; i < cbs.size(); i++) {
    //     delete cbs[i];
    // }
    // cbs.clear();
}

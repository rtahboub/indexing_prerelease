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

#include "2D/IncrementalJoinRunner2D.h"
#include "2D/JoinRunner2D.h"
#include "IndexLog.h"
#include "util/ptimer.h"
#include "util/Log.h"

#include <stdexcept>


#if defined (VTUNE_BUILD) || defined (VTUNE_QUERY) || defined (VTUNE_UPDATE) || defined (VTUNE_ALL)
#include "VtuneApi.h"
#endif


//#include "2D/continuous_join/NEJoin.h"
using namespace PSimIndex;
using namespace DBUtl;




bool IncrementalJoinRunner2D::hasNextTick() {
    return hasNext;
}


void IncrementalJoinRunner2D::updateContinuous() {

    while(workload->hasNextUpdate()) {
        UpdateRec* update = workload->nextUpdate();
        Point2D* point = update->point;
        
        switch(update->type) {

        case VEL_UPDATE:
            // join->updateVelocity(point, point->x, point->y,
            //                      update->newVx, update->newVy);

            //join->deletePoint(point);


            if(pointSet.count(point) > 0) {
                join->deletePoint(point);
            }
            if(querySet.count(point) > 0) {
                //std::cout << "Calling 1" << std::endl;
                join->deleteQuery(point);
            }


            point->vx = update->newVx;
            point->vy = update->newVy;

            if(pointSet.count(point) > 0) {
                join->insertPoint(point);
            }
            if(querySet.count(point) > 0) {
                join->insertQuery(point);
            }


            break;

        case AGG_UPDATE:
            point->data = update->newAgg;
            join->afterUpdateAgg(point);
            break;

        case POS_UPDATE:

            //join->beforeDelete(point);
            //join->deletePoint(point);


            if(pointSet.count(point) > 0) {
                join->deletePoint(point);
            }
            if(querySet.count(point) > 0) {
                //std::cout << "Calling 2" << std::endl;
                join->deleteQuery(point);
            }

                
            point->x = update->newX;
            point->y = update->newY;
            point->vx = update->newVx;
            point->vy = update->newVy;

            if(pointSet.count(point) > 0) {
                join->insertPoint(point);
            }
            if(querySet.count(point) > 0) {
                join->insertQuery(point);
            }

            break;

        case INSERT:
            workload->insertPoint(point);
            join->insertPoint(point);
            pointSet.insert(point);
            break;

        case DELETE:
            //join->beforeDelete(point);
            //std::cout << "Deleting point " << *point << std::endl;
            // if(pointSet.count(point) == 0) {
            //     std::cout << "POINT " << *point << " does not exist." << std::endl;
            // }
            DBUTL_ASSERT((pointSet.count(point) > 0));
                //if(pointSet.count(point) > 0) {
            join->deletePoint(point);
            //}
            if(querySet.count(point) > 0) {
                //std::cout << "Calling 3" << std::endl;
                join->deleteQuery(point);
            }

            pointSet.erase(point);
            querySet.erase(point);            

            workload->deletePoint(point);


            break;

        case IMPLICIT_MOVE:
            point->x = point->x + point->vx;
            point->y = point->y + point->vy;
            break;
        }

    }

}

void IncrementalJoinRunner2D::runTick(JoinCallBack<Key2D,Point2D,Point2D>* cb) { 



    //DBUTL_ASSERT((workload->hasNextTick()));
    DBUTL_ASSERT((hasNext));

    Point2D* p;
//    std::vector<Point2D*>* points;


    //IncrementalCallBack<Key2D,Point2D,Point2D>* cb = (IncrementalCallBack<Key2D,Point2D,Point2D>*) cbs[0];

    IncrementalCallBack<Key2D,Point2D,Point2D>* icb = (IncrementalCallBack<Key2D,Point2D,Point2D>*) cb;

//    std::cout << "CB" << std::endl;
//    cb->printSorted(JoinRunner2D::ltPair<Point2D,Point2D>);


    bool querySetBuilt = false;
    RegionGen2D *rg = NULL, *tmpRG = NULL;
   // std::tr1::unordered_set<Point2D*>::iterator it, it2;
    std::unordered_set<Point2D*>::iterator it, it2;
    PTimer pt; 

    //if(cbs != NULL) {
    //cb = new IncrementalCallBack<Point2D,Point2D>(true); 
    //}
    //else {
    //cb = new IncrementalCallBack<Point2D,Point2D>(false);         
    //}

    // Case in which we don't need to compare results.
    // if(cb == NULL) {
    //     cb = new IncrementalCallBack<Point2D,Point2D>(false);
    // }

    //Initialize query set at first tick. 
    if(tick == 0) {
        DBUTL_ASSERT((pointSet.size() == 0));
        DBUTL_ASSERT((querySet.size() == 0));

        workload->startTick();
        points = workload->getPoints();

        pointSet.insert(points->begin(), points->end());

//        pointSet = new std::tr1::unordered_set<Point2D*>(points->begin(), 
//                                                         points->end());

        QueryRec* query = workload->nextQuery();
        qp = (query->queryPoints == 0) ? points : query->queryPoints;
        rg = new RegionGen2D(query->regionGen);
        querySet.insert(qp->begin(), qp->end());

        join->setRegionGen(rg);
        //join->setCallBack(cb);
        join->setCallBack(icb);


        querySetBuilt = true;
        DBUTL_ASSERT((qp != NULL));
        if(workload->hasNextQuery()) {
            //std::cout << "throwing 1" << std::endl;
            throw std::logic_error("Multiple queries currently unsupported.");
        }

    }


#ifdef VTUNE_ALL
    VTResume();
#endif



//    if(points == NULL)
//        std::cout << "points is NULL" << std::endl;

    pt.start();
    join->startTick(points, qp);
    pt.finish();
    double buildTime = pt.realResult();

//    std::cout << "tick: " << tick << std::endl;


    // Perform query on index. 

//    if(queryPoints != NULL) {
    pt.start();
//    std::cout << "here1" << std::endl;
    join->updateJoinResults(/*cb*/);
//    std::cout << "here2" << std::endl;
    pt.finish();


//    }
//    else {
//        DBUTL_ASSERT((querySet->size() == 0));
//        throw std::logic_error("Empty Query");        
//    }
    //int numResults = cb->numPairs();
    int numResults = icb->numPairs();
    double queryTime = pt.realResult();

    // std::cout << "Printing icb..." << std::endl;
    // cb->printSorted(ltPair<Point2D,Point2D>);
    // std::cout << std::endl;

    // TODO: Currently this is a lower bound for update cost. We will have to
    // see if this makes a difference. 
    pt.start();
    updateContinuous();   
    pt.finish();
    double updateTime = pt.realResult();


    // std::cout << "HEREHEREHERE" << std::endl;
    // ((NEJoin<Point2D,Key2D,SumAgg2D,RegionGen2D>*)join)->printInsertDeleteSets();



//    std::cout << "CB2" << std::endl;
//    cb->printSorted(JoinRunner2D::ltPair<Point2D,Point2D>);


    if(tr != NULL) {
        tr->addTick(buildTime, queryTime, updateTime, numResults);
    }

    //Note: Invalidates queryPoints
    tick++;
    workload->endTick();
    if(!workload->hasNextTick()) {
        join->endTick(true);
//        std::cout << "setting hasNext false" << std::endl;

        hasNext = false;
        return;
    }

    workload->startTick();
    delete points;
    points = workload->getPoints();

//    queryPoints = getContQuery(pw, tmpRG, points);

    QueryRec* query = workload->nextQuery();
    qp = (query->queryPoints == 0) ? points : query->queryPoints;
    tmpRG = new RegionGen2D(query->regionGen);
//    querySet.insert(queryPoints->begin(), queryPoints->end());
//    join->setRegionGen(rg);
//    querySetBuilt = true;

    if(workload->hasNextQuery()) {
        std::cout << "throwing 2" << std::endl;
        throw std::logic_error("Multiple queries currently unsupported.");
    }


    DBUTL_ASSERT((tmpRG != NULL));

    //rg is null for some reason. 
    rg = join->getRegionGen();

    if(tmpRG == NULL || !tmpRG->equals(rg)) {
        if(rg != NULL) delete rg;
        rg = new RegionGen2D(tmpRG);
        join->setRegionGen(rg);
        delete tmpRG;
    }



    // std::cout << "HEREHEREHERE22" << std::endl;
    // ((NEJoin<Point2D,Key2D,SumAgg2D,RegionGen2D>*)join)->printInsertDeleteSets();



    if(qp != NULL) {
        //std::tr1::unordered_set<Point2D*> tmpSet(qp->begin(), qp->end());
        std::unordered_set<Point2D*> tmpSet(qp->begin(), qp->end());

        bool toDelete = false;
        for(it = querySet.begin(); it != querySet.end(); it++) {
            if(toDelete) {
                Point2D* p2 = *it2;
                std::cout << "Before erase: " << *p2 << std::endl;

                querySet.erase(it2);
                std::cout << "After erase: " << *p2 << std::endl;
                toDelete = false;

            }
                
            p = *it;
            // if(p == NULL) {

            // }
            //std::cout << "p: "<< p << std::endl;

            // point should be deleted from query points. 
            if(tmpSet.count(p) == 0) {
                std::cout << "Calling 4" << std::endl;
                join->deleteQuery(p);
                it2 = it;
                toDelete = true;
            }
        }
            
        for(it = tmpSet.begin(); it != tmpSet.end(); it++) {
            p = *it;
                
            // point should be added to query points. 
            if(querySet.count(p) == 0) {
                join->insertQuery(p);
                querySet.insert(p);
            }
        }
    }
    



    join->endTick();
    
#ifdef VTUNE_ALL
    VTPause();
#endif


    hasNext = true;
}

// std::vector<JoinCallBack<Key2D, Point2D,Point2D>*>* 
// IncrementalJoinRunner2D::getCallBacks() {
//     // COMMENTED JUST FOR DEBUGGING!!

//     // if(!copyPts) {
//     //     throw std::logic_error("copyPts must be set in IncrementalJoinRunner2D" 
//     //                            "in order to call getCallbacks");
//     // }
//     return &cbs;
// }

// void IncrementalJoinRunner2D::clearCallBacks() {
//     cbs.clear();
// }



TimingResult* IncrementalJoinRunner2D::getTimingResult() {
    return tr;
}

IncrementalJoinRunner2D::IncrementalJoinRunner2D(ContJoinType* join,
                                                 Workload2D* workload,
                                                 bool copyPts,
                                                 TimingResult* tr) {
    this->join = join;
    this->workload = workload;
    this->copyPts = copyPts;
    
    this->tr = tr;
    this->qp = NULL;
    this->points = NULL;

    if(workload->hasNextTick()) {
        hasNext = true;
    }
    else {
        hasNext = false;
    }



    // cbs.push_back(new IncrementalCallBack<Key2D,Point2D,Point2D>(copyPts));

    // cbs.push_back(getCallBackMethod(
    //                   new IncrementalCallBack<Key2D,Point2D,Point2D>(copyPts)));

    tick = 0;
}


IncrementalJoinRunner2D::~IncrementalJoinRunner2D() {
    // delete cbs[0];
    // cbs.clear();
}



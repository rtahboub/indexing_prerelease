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

#include "2D/IndexUtils2D.h"
#include "2D/Experiment2D.h"
#include "2D/join_index/StaticNestedLoops2D.h"
#include "2D/join_index/MovingNestedLoops2D.h"
#include "util/Log.h"
#include "util/ptimer.h"
#include "IndexLog.h"
#include "2D/JoinRunner2D.h"
#include <stdexcept>
#include <vector>
#include <cstdio>
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>
//#include <tr1/unordered_set>
#include <unordered_set>

using namespace PSimIndex;
using namespace DBUtl;


void Experiment2D::reportResult(ExperimentRunID* runID, TimingResult* times) {
    char filename[500];
    sprintf(filename, "%s/%s", folder, runID->getName());
    TimingResult::serialize(filename, times);
    std::cout << "Wrote file " << filename << std::endl;
}

void Experiment2D::run() {
    // std::vector<JoinMethod*>::iterator jIt;
    // std::vector<std::vector<Workload2D*>*>::iterator wcIt;
    // std::vector<Workload2D*>::iterator wIt, wIt2;
    // std::vector<double*>::iterator ipIt, wpIt;
    ExperimentRunID* rID;
    TimingResult* timing;

    
    // std::map<const char*,int,classcomp>::iterator it;

    // for(int i = 0; i < workloads.size(); i++) {
    //     std::cout <<  workloadClassNames[i] << std::endl;
    // }

    // for(it = workloadNum.begin(); it != workloadNum.end(); it++) {
    //     std::cout << it->first << " -> " << it->second << std::endl;

    // }

    for(int i = 0; i < indices.size(); i++) {
        JoinMethod* join = indices[i];
        const char* joinName = join->getName();
        int joinNum = indexNum[joinName];

        for(int j = 0; j < workloads.size(); j++) {

            std::vector<Workload2D*>* workloadClass = workloads[j];
            const char* workloadName = workloadClassNames[j];

            int startTick = workloadStartTicks[j];
            int currWorkloadNum = workloadNum[workloadName];
            // std::cout << "workloadName: " << workloadName << " " 
            //           << "workloadNum: " << currWorkloadNum << " "
            //           << workloadNum.count(workloadName) << std::endl;


            rID = new ExperimentRunID(joinNum, currWorkloadNum, 
                                      joinName, workloadName, 
                                      indexParamVals[i], indexParams.size(), 
                                      workloadParamVals[j], workloadParams.size());


            timing = new TimingResult(rID,
                                      startTick,
                                      experimentName,
                                      &indexParams,
                                      &workloadParams,
                                      false);

            for(int k = 0; k < workloadClass->size(); k++) {
                polluteCache();
                Workload2D* workload = workloadClass->at(k);

                std::cout << "\tRunning join on " << workload->getName() << " trial " << k << std::endl;
                JoinRunner2D::runJoinOnWorkload(join, workload, timing);
                workload->reset();
                
                if(k != workloadClass->size() - 1) {
                    timing->nextTrial();
                }
            }
            reportResult(rID, timing);
            delete timing;
            delete rID;
        }
    }

}






void Experiment2D::polluteCache() {
    int memSize = 16 * 1024 * 1024; //16 MB
    
    void * b = malloc(memSize);
    memset(b, 17, memSize);
    free(b);
}




Experiment2D::~Experiment2D() {
    std::vector<JoinMethod*>::iterator jIt;
    std::vector<std::vector<Workload2D*>*>::iterator wcIt;
    std::vector<Workload2D*>::iterator wIt;

    for(int i = 0; i < indexParamVals.size(); i++) {
        if(indexParamVals[i] != NULL) {
            delete [] indexParamVals[i];
        }
    }
    indexParamVals.clear();


    for(int i = 0; i < workloadParamVals.size(); i++) {
        if(workloadParamVals[i] != NULL) {
            delete [] workloadParamVals[i];
        }
    }
    workloadParamVals.clear();

    for(wcIt = workloads.begin(); wcIt != workloads.end(); wcIt++) {
        std::vector<Workload2D*>* wClass = *wcIt;

        for(wIt = wClass->begin(); wIt != wClass->end(); wIt++) {
            delete *wIt;
        }
        delete wClass;
    }




    for(jIt = indices.begin(); jIt != indices.end(); jIt++) {
        delete *jIt;
    }
}

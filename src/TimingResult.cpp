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

#include "TimingResult.h"
#include <cstdio>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include "util/Log.h"
#include "IndexLog.h"

using namespace PSimIndex;
using namespace DBUtl;


std::ostream& PSimIndex::operator<<(std::ostream &out, 
                                    const PSimIndex::ExperimentRunID& rID) {
    out << "ERunID(" << std::setiosflags(std::ios::fixed) << std::setprecision(2) 
        << rID.getIndexName() << ", " << rID.getIndexNum() << ", " 
        << rID.getWorkloadName() << ", " << rID.getWorkloadNum() << ")" ;
    return out;
}


void TimingResult::serialize(const char* filename, TimingResult* tr) {
    //std::cout << "file: " << filename << std::endl;

    FILE* file = fopen(filename, "w");
    
    if(file == 0) {
        throw std::logic_error("Couldn't find file");
    }

    fprintf(file, "indexName = %s %d\n", tr->getIndexName(), tr->getIndexNum());
    fprintf(file, "workloadName = %s %d\n", tr->getWorkloadName(), tr->getWorkloadNum());
    fprintf(file, "experimentName = %s\n", tr->getExperimentName());
    fprintf(file, "num. index params = %d\n", (int) tr->getNumIndexParams());

    for(int i = 0; i < tr->getNumIndexParams(); i++) {
        fprintf(file, "%s = %lf\n", tr->getIndexParamName(i), tr->getIndexParamVal(i));
    }

    fprintf(file, "num. workload params = %d\n", (int) tr->wParams->size());

    for(int i = 0; i < tr->getNumWorkloadParams(); i++) {
        fprintf(file, "%s = %lf\n", tr->getWorkloadParamName(i), tr->getWorkloadParamVal(i));
    }

    int numTicks = (int) tr->getNumTicks();
    fprintf(file, "numTrials = %d\n", tr->getNumTrials());
    fprintf(file, "numTicks = %d\n", numTicks);
    fprintf(file, "startTick = %d\n", tr->getStartTick());

//    std::cout << "numTicks: " << numTicks << std::endl;

    for(int i = 0; i < numTicks; i++) {

        //std::cout << "getNumResults: " << tr->getNumResults(0, i);
        //std::cout << " getTickNumResultsAvg : " << tr->getAvgTickNumResults(i) << std::endl;
        //std::cout << "numTrials: " << tr->getNumTrials() << std::endl;
        for(int j = 0; j < tr->getNumTrials(); j++) {
            fprintf(file, "[%lf %lf %lf] %ld ", 
                    tr->getTime(j, i, BUILD),
                    tr->getTime(j, i, QUERY),
                    tr->getTime(j, i, UPDATE),
                    tr->getNumResults(j, i));
        }

        fprintf(file, "\n");
    }
    fclose(file);
}


TimingResult* TimingResult::deserialize(const char* filename) {
    ///*LOG(DEBUG)*/std::cout << "In deserialize, filename = " << filename << std::endl;

    FILE* file = fopen(filename, "r");

    LOG(DEBUG) << "In deserialize, Opened file = " << filename << std::endl;    

    TimingResult* ans;

    char* indexName = new char[100];
    char* workloadName = new char[100];
    int paramInt;
    char* paramName = new char[100];

    char* experimentName = new char[100];
    double paramVal;

    int numTrials, numTicks, indexNum, workloadNum, startTick;
    double build, query, update;
    int read;
    int numResults;

    read = fscanf(file, "indexName = %s %d\n", indexName, &indexNum);
    read = fscanf(file, "workloadName = %s %d\n", workloadName, &workloadNum);

    //std::cout << "indexName: " << indexName << std::endl;
    //std::cout << "workloadName: " << workloadName << std::endl;

    //read = fscanf(file, "traceName = %s\n", traceName);
    read = fscanf(file, "experimentName = %s\n", experimentName);
    //std::cout << "experimentName: " << experimentName << std::endl;


    int numIParams, numWParams;
    std::vector<const char*>* iParams = new std::vector<const char*>();
    std::vector<const char*>* wParams = new std::vector<const char*>();
    char* name;

    read = fscanf(file, "num. index params = %d\n", &numIParams);

    //std::cout << "numIParams: " << numIParams << std::endl;
    double* iParamVals = new double[numIParams];




    for(int i = 0; i < numIParams; i++) {
        name = new char[100];
        read = fscanf(file, "%s = %lf\n", name, &iParamVals[i]);
        iParams->push_back(name);
    }



    fscanf(file, "num. workload params = %d\n", &numWParams);
    //std::cout << "numWParams: " << numWParams << std::endl;

    double* wParamVals = new double[numIParams];
    for(int i = 0; i < numWParams; i++) {
        name = new char[100];
        fscanf(file, "%s = %lf\n", name, &wParamVals[i]);
        wParams->push_back(name);
    }

    read = fscanf(file, "numTrials = %d\n", &numTrials);
    //std::cout << "numTrials: " << numTrials << std::endl;

    read = fscanf(file, "numTicks = %d\n", &numTicks);
    //std::cout << "numTicks: " << numTrials << std::endl;

    read = fscanf(file, "startTick = %d\n", &startTick);
    //std::cout << "startTick: " << startTick << std::endl;

    DBUTL_ASSERT((read > 0));


    ExperimentRunID* rID = new ExperimentRunID(indexNum, 
                                               workloadNum, 
                                               indexName, 
                                               workloadName, 
                                               iParamVals, 
                                               iParams->size(), 
                                               wParamVals, 
                                               wParams->size());


    ans = new TimingResult(rID, 
                           startTick, 
                           experimentName, 
                           iParams,
                           wParams,
                           true);

    // ans = new TimingResult(indexName, 
    //                        startTick, 
    //                        indexNum, 
    //                        experimentName, 
    //                        iParams,
    //                        wParams,
    //                        iParamVals,
    //                        wParamVals,
    //                        true);


    
    std::vector<Trial*> trials;
    trials.reserve(numTicks);
    for(int i = 0; i < numTrials; i++) {
        trials.push_back(new Trial());
    }
    
    for(int i = 0; i < numTicks; i++) {
        for(int j = 0; j < numTrials; j++) {
            read = fscanf(file, "[%lf %lf %lf] %d ", 
                          &build, &query, &update, &numResults);

            DBUTL_ASSERT((read > 0));
            //ans->addTiming(i, build, query, update, numResults);
            trials[j]->push_back(new TickTrial(build, query, update, numResults));
        }
        read = fscanf(file, "\n");
        //DBUTL_ASSERT((read > 0));
    }

    for(int i = 0; i < numTrials; i++) {
        ans->addTrial(trials[i]);
    }

    fclose(file);
    return ans;
}


void TimingResult::addTick(double build, double query, 
                           double update, int numResults) {
    addTick(new TickTrial(build, query, update, numResults));
}
void TimingResult::addTick(TickTrial* t) {
    if(trials.size() == 0)
        trials.push_back(new Trial());

    trials[currTrial]->push_back(t);
}

void TimingResult::nextTrial() {
    if(currTrial == 0) {
        numTicks = trials[currTrial]->size();
    }
    else {
        DBUTL_ASSERT((numTicks == trials[currTrial]->size()));
    }
    trials.push_back(new Trial());
    currTrial++;
}


void TimingResult::addTrial(Trial* trial) {
    if(trials.size() == 0) {
        numTicks = trial->size();        
    }
    else {
        DBUTL_ASSERT((getNumTicks() == trial->size()));
    }

    trials.push_back(trial);
}




double TimingResult::getTime(int trial, int tick, TimeComponent comp) {
    DBUTL_ASSERT((trial < trials.size()));
    DBUTL_ASSERT((tick < trials[trial]->size()));
    return trials[trial]->at(tick)->getTime(comp);
}

long TimingResult::getNumResults(int trial, int tick) {
//    std::cout << "trial: " << trial << " tick: " << tick 
//              << " numTrials: " << getNumTrials() 
//              << " getNumTicks: " << getNumTicks() 
//              << std::endl;

    DBUTL_ASSERT((trial < trials.size()));
    DBUTL_ASSERT((tick < trials[trial]->size()));
    return trials[trial]->at(tick)->numResults;
}

//------------------------------------------------------------------------------
// TRIAL METHODS
//------------------------------------------------------------------------------

 
double TimingResult::getTrialTime(int trial, TimeComponent comp) {
    double time = 0.0;
    for(int i = startTick; i < getNumTicks(); i++) {
        time += getTime(trial, i, comp);
    }
    return time;
}

double TimingResult::getAvgTrialTime(TimeComponent comp) {
    double avg = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        avg += getTrialTime(i, comp);
    }
    return (avg / trials.size());
}

double TimingResult::getMinTrialTime(TimeComponent comp) {
    double min = std::numeric_limits<double>::max();
    for(int i = 0; i < trials.size(); i++) {
        double t = getTrialTime(i, comp);
        if(t < min) { 
            min = t; 
        }  
    }
    return min;
}

double TimingResult::getMaxTrialTime(TimeComponent comp) {
    double max = std::numeric_limits<double>::min();
    for(int i = 0; i < trials.size(); i++) {
        double t = getTrialTime(i, comp);
        if(t > max) { 
            max = t; 
        }  
    }
    return max;
}

double TimingResult::getStdDevTrialTime(TimeComponent comp) {
    double avg = getAvgTrialTime(comp);
    double sum = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        double x = getTrialTime(i, comp);
        sum += (x - avg) * (x - avg);
    }
    return sqrt(sum / trials.size());
}
        
long TimingResult::getTrialNumResults(int trial) {
    long nr = 0;
    for(int i = startTick; i < getNumTicks(); i++) {
        nr += getNumResults(trial, i);
    }
    return nr;
}
double TimingResult::getAvgTrialNumResults() {
    double avg = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        avg += getTrialNumResults(i);
    }
    return (avg / trials.size());
}

long TimingResult::getMinTrialNumResults() {
    long min = std::numeric_limits<long>::max();
    for(int i = 0; i < trials.size(); i++) {
        long t = getTrialNumResults(i);
        if(t < min) { 
            min = t; 
        }  
    }
    return min;
}
long TimingResult::getMaxTrialNumResults() {
    int max = std::numeric_limits<long>::min();
    for(int i = 0; i < trials.size(); i++) {
        long t = getTrialNumResults(i);
        if(t > max) { 
            max = t; 
        }  
    }
    return max;
}

double TimingResult::getStdDevTrialNumResults() {
    double avg = getAvgTrialNumResults();
    double sum = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        double x = getTrialNumResults(i);
        sum += (x - avg) * (x - avg);
    }
    return sqrt(sum / trials.size());
}

//------------------------------------------------------------------------------
// TICK METHODS
//------------------------------------------------------------------------------

double TimingResult::getAvgTickTime(int tick, TimeComponent comp) {
    double sum = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        sum += getTime(i, tick, comp);
    }
    return (sum / trials.size());
}

double TimingResult::getMinTickTime(int tick, TimeComponent comp) {
    double min = std::numeric_limits<double>::max();
    for(int i = 0; i < trials.size(); i++) {
        double t = getTime(i, tick, comp);
        if(t < min) { 
            min = t; 
        }  
    }
    return min;
}

double TimingResult::getMaxTickTime(int tick, TimeComponent comp) {
    double max = std::numeric_limits<double>::min();
    for(int i = 0; i < trials.size(); i++) {
        double t = getTime(i, tick, comp);
        if(t > max) { 
            max = t; 
        }  
    }
    return max;
}

double TimingResult::getStdDevTickTime(int tick, TimeComponent comp) {
    double avg = getAvgTickTime(tick, comp);
    double sum = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        double x = getTime(i, tick, comp);
        sum += (x - avg) * (x - avg);
    }
    return sqrt(sum / trials.size());
}

double TimingResult::getAvgTickNumResults(int tick) {
    double sum = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        sum += getNumResults(i, tick);
    }
    return (sum / trials.size());

}

long TimingResult::getMinTickNumResults(int tick) {
    long min = std::numeric_limits<long>::max();
    for(int i = 0; i < trials.size(); i++) {
        long nr = getNumResults(i, tick);
        if(nr < min) { 
            min = nr; 
        }  
    }
    return min;
}

long TimingResult::getMaxTickNumResults(int tick) {
    long max = std::numeric_limits<long>::min();
    for(int i = 0; i < trials.size(); i++) {
        long nr = getNumResults(i, tick);
        if(nr > max) { 
            max = nr; 
        }  
    }
    return max;
}

double TimingResult::getStdDevTickNumResults(int tick) {
    double avg = getAvgTickNumResults(tick);
    double sum = 0.0;
    for(int i = 0; i < trials.size(); i++) {
        double x = getNumResults(i, tick);
        sum += (x - avg) * (x - avg);
    }
    return sqrt(sum / trials.size());
}



TimingResult::~TimingResult() {
    for(int i = 0; i < trials.size(); i++) {
        Trial* t = trials[i];
        for(int j = 0; j < t->size(); j++) {
            delete t->at(j);
        }
        delete t;
    }
}

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

#ifndef _TIMING_RESULT_2D_H_
#define _TIMING_RESULT_2D_H_

#include <vector>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include "util/Log.h"

namespace PSimIndex {

    /**
     * TODO: Consider putting this in a separate file. 
     *
     * This encapsulates the basic data needed to identify an experiment
     * including the index and workload names, ids, and parameters. These were
     * originally passed separately to the TimingResult constructor, but they
     * have changed over time, so I have found it easier to combine them in a
     * class.
     */
    class ExperimentRunID {
    private:
        int indexNum, workloadNum;
        const char* indexName;
        const char* workloadName; 
        double* indexParams;
        int numIndexParams;
        int numWorkloadParams;
        double* workloadParams;
        char name[500];

    public:
        int getIndexNum() const {
            return indexNum;
        }

        int getWorkloadNum() const {
            return workloadNum;
        }
        
        const char* getIndexName() const {
            return indexName;
        }

        const char* getWorkloadName() const {
            return workloadName;
        }

        double getIndexParam(int i) const {
            DBUTL_ASSERT((i < numIndexParams));
            return indexParams[i];
        }

        double getWorkloadParam(int i) const {
            DBUTL_ASSERT((i < numWorkloadParams));
            return workloadParams[i];
        }

        const char* getName() const {
            return name;
        }

        ExperimentRunID(int indexNum, int workloadNum, 
                        const char* indexName, const char* workloadName,
                        double* indexParams, int numIndexParams,
                        double* workloadParams, int numWorkloadParams)
            : indexNum(indexNum), workloadNum(workloadNum), 
              indexName(indexName), workloadName(workloadName), 
              indexParams(indexParams), numIndexParams(numIndexParams),
              workloadParams(workloadParams), numWorkloadParams(numWorkloadParams) {

            strcpy(name, indexName);

            char tmp[200];
            for(int i = 0; i < numIndexParams; i++) {
                sprintf(tmp, "_%.2f", indexParams[i]);
                strcat(name, tmp);
            }
            
            strcat(name, workloadName);

            for(int i = 0; i < numWorkloadParams; i++) {
                sprintf(tmp, "_%.2f", workloadParams[i]);
                strcat(name, tmp);
            }
        }

        ~ExperimentRunID() {}
    };


    std::ostream& operator<<(std::ostream &out, const ExperimentRunID& rID);





    /**
     * The TOTAL time of a trial is divided into BUILD, QUERY, and UPDATE
     * times. Statistics can be computed on any component.
     */
    enum TimeComponent { BUILD = 0, QUERY = 1, UPDATE = 2, TOTAL = 3};

    /**
     * Stores timing information for a single index structure on a single
     * workload. Maintains the times to build, query, and update each index
     * for each tick. Also provides methods to compute statistics for
     * plotting.  
     *
     * TODO: Should we be able to pass a startTick to each get method?
     */
    class TimingResult {
    private:

        /**
         * Stores timing information for a single trial of a single tick. Each
         * trial is represented by four times: build, query, total, and
         * update. These correspond to the four TimeComponents. 
         *
         * numResults is the total number of join results preduced by all
         * queries for this tick. This is primarily useful for debugging and
         * determining the output cost. 
         */
        struct TickTrial {
            long numResults;
            double times[4];
            double getTime(TimeComponent comp) { return times[(int) comp]; }
            TickTrial(double build, double query, double update, int numResults) {
                times[BUILD] = build;
                times[QUERY] = query;
                times[UPDATE] = update;
                times[TOTAL] = build + query + update;
                this->numResults = numResults;
            }
        };

        typedef std::vector<TickTrial*> Trial;
        std::vector<Trial*> trials;
        int numTicks;
        int currTrial;

        int startTick;

        //const char* indexName;
        //int indexNum;
        const char* experimentName;
        bool deserialized;

        ExperimentRunID* rID;
        std::vector<const char*>* iParams;
        std::vector<const char*>* wParams;

        //Used internally for deserialization. Should not be called once
        //nextTrial has been called.
        void addTrial(Trial* trial);
    public:

        //double* indexParamVals;
        //double* workloadParamVals;

        /**
         * Writes the specified TimingResult object to the specified
         * file in a human readable text format.  
         */
        static void serialize(const char* file, TimingResult* tr);


        /**
         * Reads the specified file created using the serialize method and
         * returns the deserialized TimingResult object. 
         */
        static TimingResult* deserialize(const char* file);


        void addTick(double build, double query, double update, int numResults);
        void addTick(TickTrial* t);
        void nextTrial();






        /**
         * Returns the time/numResults spent in a specific tick of a specific
         * trial.
         */
        double getTime(int trial, int tick, TimeComponent comp = TOTAL);
        long getNumResults(int trial, int tick);

        /**
         * Returns timings/numReslts aggregated over trials,
         * i.e. getAvgTrialTime returns the average time of a complete trial.
         */
        double getTrialTime(int trial, TimeComponent comp = TOTAL);
        double getAvgTrialTime(TimeComponent comp = TOTAL);
        double getMinTrialTime(TimeComponent comp = TOTAL);
        double getMaxTrialTime(TimeComponent comp = TOTAL);
        double getStdDevTrialTime(TimeComponent comp = TOTAL);
        
        long getTrialNumResults(int trial);
        double getAvgTrialNumResults();
        long getMinTrialNumResults();
        long getMaxTrialNumResults();
        double getStdDevTrialNumResults();


        /**
         * Returns timings/numReslts aggregated over ticks, i.e. getAvgTickTime returns
         * the time spent in the specified tick averaged over all trials.
         */
        double getAvgTickTime(int tick, TimeComponent comp = TOTAL);
        double getMinTickTime(int tick, TimeComponent comp = TOTAL);
        double getMaxTickTime(int tick, TimeComponent comp = TOTAL);
        double getStdDevTickTime(int tick, TimeComponent comp = TOTAL);

        double getAvgTickNumResults(int tick);
        long getMinTickNumResults(int tick);
        long getMaxTickNumResults(int tick);
        double getStdDevTickNumResults(int tick);




        ExperimentRunID* getExperimentRun() const {
            return rID;
        }
        
        const char* getIndexName() const { 
            return rID->getIndexName(); 
        }

        const char* getWorkloadName() const { 
            return rID->getWorkloadName(); 
        }


//        void setIndexName(const char* indexName) { 
//            this->indexName = indexName; 
//        }


        int getIndexNum() const { 
            return rID->getIndexNum(); 
        }

        int getWorkloadNum() const { 
            return rID->getWorkloadNum(); 
        }
        

        const char* getIndexParamName(int i) const {
            return iParams->at(i);
        }

        int getNumIndexParams() const {
            return iParams->size();
        }

        double getIndexParamVal(int i) const {
            return rID->getIndexParam(i);
        }

        const char* getWorkloadParamName(int i) const {
            return wParams->at(i);
        }

        int getNumWorkloadParams() const {
            return wParams->size();
        }

        double getWorkloadParamVal(int i) const {
            return rID->getWorkloadParam(i);
        }



//        void setIndexNum(int indexNum) {this->indexNum = indexNum; }

        const char* getExperimentName() const { return experimentName; }

        // void setExperimentName(const char* experimentName) { 
        //     this->experimentName = experimentName; 
        // }
        
        //TODO: Fix this.
        int getNumTicks() const { 
            return (getNumTrials() > 1) ? numTicks : trials[0]->size();
        }

        int getStartTick() const {
            return startTick;
        }

        int getNumTrials() const {
            return trials.size();
        }



        TimingResult(ExperimentRunID* rID = NULL, 
                     int startTick = 0,
                     const char* experimentName = "default", 
                     std::vector<const char*>* iParams = NULL,
                     std::vector<const char*>* wParams = NULL,
                     bool deserialized = false):
            rID(rID),
            startTick(startTick),
            experimentName(experimentName),            
            iParams(iParams),
            wParams(wParams),
            deserialized(deserialized) {

            currTrial = 0;
            numTicks = -1;
        }






//         TimingResult(const char* indexName = "default", 
//                      int startTick = 0, 
//                      int indexNum = -1,
//                      const char* experimentName = "default", 
//                      std::vector<const char*>* iParams = NULL,
//                      std::vector<const char*>* wParams = NULL,
//                      double* indexParamVals = NULL,
//                      double* workloadParamVals = NULL, 
//                      bool deserialized = false):
//             indexName(indexName),
//             startTick(startTick),
//             indexNum(indexNum),
//             experimentName(experimentName),
//             iParams(iParams),
//             wParams(wParams),
//             indexParamVals(indexParamVals),
//             workloadParamVals(workloadParamVals),
//             deserialized(deserialized) {

//             currTrial = 0;
//             numTicks = -1;
// //            trials.push_back(new Trial());
//         }

        ~TimingResult();


    };

}

#endif

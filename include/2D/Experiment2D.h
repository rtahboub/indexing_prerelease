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

#ifndef _EXPERIMENT_2D_H_
#define _EXPERIMENT_2D_H_

#include "TimingResult.h"
#include "JoinCallBack.h"
#include "2D/Workload2D.h"
#include "2D/SpatialJoin2D.h"
#include "2D/MovingSpatialJoin2D.h"
#include "2D/StaticSpatialJoin2D.h"
#include "2D/MovingSpatialIndex2D.h"
#include "2D/StaticSpatialIndex2D.h"
#include "2D/ContinuousMovingJoin2D.h"

#include <vector>
#include <map>
#include <cstring>

namespace PSimIndex {
    

    /**
     * Represents a single experiment on 2D spatial joins. An experiment is a
     * specific set of joins and workloads that should be compared. Typically
     * these would include all of the data necessary to produce a single
     * graph.
     *
     * To implement an Experiment, one must implement the init method, which
     * populates the vectors of indices and workloads to test. Additionally,
     * one can override the reportResult, which is called for every
     * index-workload pair, and the close method, which is called at the end
     * of the experiment. 
     *
     * The Experiment2D class also contains static methods for comparing two
     * indices on a single workload and timing a single index on a single
     * workload.
     */
    class Experiment2D {

    // ----------------------------------------------------------------
    // Private static methods for running a join on a workload. 
    // ----------------------------------------------------------------
    private:

        static void polluteCache(); 

    // ----------------------------------------------------------------
    // Public static methods for testing and running joins. 
    // ----------------------------------------------------------------
    public:



    // ----------------------------------------------------------------
    //  Private instance variables and methods. 
    // ----------------------------------------------------------------
    private:
        int currIndexNum, currWorkloadNum;                 

        struct classcomp {
            bool operator() (const char* const& c1, const char* const& c2) const {
                return (strcmp(c1, c2) < 0);
            }
        };


    // ----------------------------------------------------------------
    //  Protected instance variables and methods. 
    // ----------------------------------------------------------------
    protected:


        std::vector<JoinMethod*> indices;


//        std::vector<Workload2D*> workloads;

        // The vectors represent "workload classes" corresponding to workloads
        // executed as different trials
        std::vector<std::vector<Workload2D*>*> workloads;
        std::vector<const char*> workloadClassNames;
        std::vector<int> workloadStartTicks;

        std::vector<double*> indexParamVals, workloadParamVals;
        std::vector<const char*> indexParams, workloadParams;



        std::map<const char*,int,classcomp> indexNum;
        std::map<const char*,int,classcomp> workloadNum;

        int numTrials;


        // The first tick that will be included in aggregate timings. This
        // allows us to omit a prefix of ticks in order to avoid startup
        // effects. By default, all ticks are included.
        //int startTick;


        const char* folder;
        const char* experimentName;

        
        // inline void setStartTick(int startTick) {
        //     this->startTick = startTick;
        // }




        inline void addIndex(JoinMethod* index, double* paramArray = NULL) {
            indices.push_back(index); 
            indexNum[index->getName()] = currIndexNum++;
            indexParamVals.push_back(paramArray);
        }

        inline void addIndex(JoinMethod* index, double paramVal) {
            double* paramArray = new double[1];
            paramArray[0] = paramVal;
            addIndex(index, paramArray);
        }

        /**
         * Inserts a new index to be compared. 
         */
        inline void addIndex(JoinType* index, double* paramArray = NULL) { 
            addIndex(getJoinMethod(index), paramArray);
        }

        /**
         * Inserts a new index to be compared. 
         */
        inline void addIndex(ContJoinType* join, double* paramArray = NULL) { 
            addIndex(getJoinMethod(join), paramArray);            
        }


        /**
         * Inserts a new workload to be compared.
         */
        inline void addWorkload(Workload2D* w, int startTick, double* paramArray = NULL) { 

            std::vector<Workload2D*>* workloadClass = new std::vector<Workload2D*>();
            workloadClass->push_back(w);
            workloads.push_back(workloadClass); 
            workloadStartTicks.push_back(startTick);
            workloadClassNames.push_back(w->getName());
            workloadNum[w->getName()] = currWorkloadNum++;
            workloadParamVals.push_back(paramArray);
        }

        inline void addWorkload(Workload2D* w, int startTick, double paramVal) {
            double* paramArray = new double[1];
            paramArray[0] = paramVal;
            addWorkload(w, startTick, paramArray);
        }



        inline void addWorkload(std::vector<Workload2D*>* workloadClass, 
                                const char* className,
                                int startTick,
                                double* paramArray = NULL) {
            workloads.push_back(workloadClass); 
            workloadClassNames.push_back(className);
            workloadStartTicks.push_back(startTick);
            workloadNum[className] = currWorkloadNum++;

            workloadParamVals.push_back(paramArray);
        }


        inline void addWorkload(std::vector<Workload2D*>* workloadClass, 
                                const char* className,
                                int startTick,
                                double paramVal) {
            double* paramArray = new double[1];
            paramArray[0] = paramVal;
            addWorkload(workloadClass, className, startTick, paramArray);
        }


        inline void addIndexParam(const char* param) {
            indexParams.push_back(param);
        }

        inline void addWorkloadParam(const char* param) {
            workloadParams.push_back(param);
        }

        /**
         * Callback function that gets timing results for each index/workload
         * pair. The default implementation simply serializes the TimingResult
         * object to a file. 
         */
        // virtual void reportResult(//const char* joinName,
        //                           ExperimentRunID* runID,
        //                           //std::vector<Workload2D*>* workloadClass, 
        //                           TimingResult* times);



        /**
         * Callback function that gets timing results for each index/workload
         * pair. The default implementation simply serializes the TimingResult
         * object to a file. 
         */
        virtual void reportResult(ExperimentRunID* runID, TimingResult* times);


    // ----------------------------------------------------------------
    //  Public instance methods for running an experiment. 
    // ----------------------------------------------------------------
    public:

        /**
         * Returns the number of trials to be executed for each join/workload
         * pair. 
         */
        inline int getNumTrials() { return getNumTrials(); }

        /**
         * Sets the number of trials to be executed for each join/workload
         * pair.
         */
        inline void setNumTrials(int nt) { numTrials = nt; }

        /**
         * Starts the experiment. This method should be overridden by every
         * experiment to add the appropriate indices and workloads using
         * addIndex and addWorkload;
         */
        virtual void init() = 0;

        /**
         * Runs the experiment. For each index and workload, runs the index on
         * the workload numTrials times and reports the timing results using
         * the reportResult method. 
         */
        void run();

        /**
         * Ends the experiment. Does nothing by default, but can be overridden
         * to do additional aggregate processing.
         */
        virtual void close() {}

        /**
         * Default constructor. 
         */
        Experiment2D() { 
            numTrials = 1; 
            currIndexNum = 0;
            currWorkloadNum = 0;
        }

        /**
         * Destructor. Deletes all workloads and indices. 
         */
        virtual ~Experiment2D();

    };

    // ----------------------------------------------------------------
    // Template Specialization Prototypes
    // ----------------------------------------------------------------

    // template<>
    // bool Experiment2D::ltPair<Point2D, Point2D>(
    //     std::pair<Point2D*,Point2D*>* o1,
    //     std::pair<Point2D*,Point2D*>* o2
    //     );

    // template<>
    // bool Experiment2D::ltPair<Point2D, SumAgg2D>(
    //     std::pair<Point2D*,SumAgg2D*>* o1,
    //     std::pair<Point2D*,SumAgg2D*>* o2
    //     );
}

#endif /* _EXPERIMENT_2D_H_ */

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

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <dirent.h>
#include "TimingResult.h"
#include "IndexLog.h"
#include "util/Log.h"

using namespace PSimIndex;
using namespace DBUtl;

namespace /* anon */ {

    class ArrayComp { 
    private:
        int size;
    public:
        ArrayComp(int size) : size(size) {}

        bool operator() (const double* lhs, const double* rhs) const {
            for(int i = 0; i < size; i++) {
                if(lhs[i] < rhs[i]) {
                    return true;
                }
                else if(lhs[i] > rhs[i]) {
                    return false;
                }
            }
            return false;
        }
    };

    void throwUsage(const char* msg) {
        throw std::logic_error(msg);
    }

    // Taken from Al's code. Should use DBUtl/CmdLine, but the logging is
    // wrong...
    char const * suffixAfterMatch(const char *s, const char *flagArg) {
        size_t n = strlen(flagArg);
        char const * ans = ((strncmp(s, flagArg, n) == 0) ? (s+n) : 0);
        return ans;
    }

    bool trlt(TimingResult* tr1, TimingResult* tr2) {
//        DBUTL_ASSERT(strcmp(tr1->getParamName(), tr2->getParamName()) == 0);
        return(tr1->getIndexNum() < tr2->getIndexNum() 
               || ((tr1->getIndexNum() == tr2->getIndexNum())
                   && tr1->getWorkloadNum() < tr2->getWorkloadNum()));
    }

    //enum PlotType { AVG, AVG_BREAKDOWN };

    typedef std::map<double*, std::vector<TimingResult*>*, ArrayComp> MapType;






    MapType* createMap(const char* dirName,
                       char* experName 
                       /*std::vector<const char*>*& iParams, 
                         std::vector<const char*>*& wParams*/) {


        std::cout << "starting createMap: " << std::endl;

        struct dirent* dirRec;

        DIR* dir = opendir(dirName);
        if(dir == NULL) {
            throw std::logic_error("Couldn't open directory");
        }
        char filename[200];
        //FILE* timefile;
        TimingResult* tr;

        MapType* trMap = NULL;
        double* key;
        int iSize, totSize;

        while(dirRec = readdir(dir)) {
            //filter hidden files
            if('.' != *dirRec->d_name) {
                /*LOG(DEBUG)*/ std::cout << "Reading file: " << dirRec->d_name << std::endl;

                sprintf(filename, "%s%s", dirName, dirRec->d_name);

                /*LOG(DEBUG)*/std::cout << "New filename: " << filename << std::endl;
                //timefile = fopen(filename, "r");
                tr = TimingResult::deserialize(filename);

                iSize = tr->getNumIndexParams();
                totSize = iSize + tr->getNumWorkloadParams();

                std::cout << "iSize = " << iSize << std::endl;
                std::cout << "totSize = " << iSize << std::endl;

                // Construct new map of appropriate size on the first pass
                // through the loop and set experimentName. and iParams/wParams.
                if(trMap == NULL) {
                    ArrayComp comp(totSize);
                    trMap = new MapType(comp);

                    strcpy(experName, tr->getExperimentName());
                    //iParams = tr->iParams;
                    //wParams = tr->wParams;

                }

                key = new double[totSize];

                for(int i = 0; i < iSize; i++) {
                    key[i] = tr->getIndexParamVal(i);
                }
                for(int i = iSize; i < totSize; i++) {
                    key[i] = tr->getWorkloadParamVal(i - iSize);
                }

                // memcpy(key, tr->indexParamVals, sizeof(double) * iSize);
                // memcpy(&(key[iSize]), tr->workloadParamVals, 
                //        sizeof(double) * (totSize - iSize));

                if(trMap->count(key) == 0) {
                    (*trMap)[key] = new std::vector<TimingResult*>();
                    (*trMap)[key]->push_back(tr);
                }
                else {
                    (*trMap)[key]->push_back(tr);
                }
            }
        }

        MapType::iterator it = trMap->begin();
        std::vector<TimingResult*>* vec;
        for(it = trMap->begin(); it != trMap->end(); it++) {
            vec = it->second;
            std::sort(vec->begin(), vec->end(), trlt);
        }
        
        closedir(dir);
        return trMap;
    }



    // void printCategoricalAttrFile(FILE* outfile, MapType* trMap, int targetTrial) {

    //     MapType::iterator it;
    //     std::vector<TimingResult*>* vec;
    //     std::vector<TimingResult*>::iterator vIt;

        
    //     it = trMap->begin();
    //     vec = it->second;


    //     std::sort(vec->begin(), vec->end(), trlt);

    //     TimingResult* first = vec->at(0);

    //     for(int i = 0; i < first->iParams->size(); i++) {
    //         fprintf(outfile, "%s ", first->iParams->at(i));
    //     }
    //     for(int i = 0; i < first->wParams->size(); i++) {
    //         fprintf(outfile, "%s ", first->wParams->at(i));
    //     }



    // }


    void printAvgFile(FILE* out, MapType* trMap, int targetTrial) {

        MapType::iterator it;
        std::vector<TimingResult*>* vec;
        std::vector<TimingResult*>::iterator vIt;

        it = trMap->begin();
        vec = it->second;


        LOG(DEBUG) << std::endl;

        std::sort(vec->begin(), vec->end(), trlt);

        TimingResult* first = vec->at(0);
        
        for(int i = 0; i < first->getNumIndexParams(); i++) {
            fprintf(out, "%s ", first->getIndexParamName(i));
        }
        for(int i = 0; i < first->getNumWorkloadParams(); i++) {
            fprintf(out, "%s ", first->getWorkloadParamName(i));
        }
        int totSize = first->getNumIndexParams() + first->getNumWorkloadParams();
//        int totSize = first->iParams->size() + first->wParams->size();

        for(vIt = vec->begin(); vIt != vec->end(); vIt++) {
            fprintf(out, "%s ", (*vIt)->getIndexName());
        }
        fprintf(out, "\n");

        for(it = trMap->begin(); it != trMap->end(); it++) {

            for(int i = 0; i < totSize; i++) {
                fprintf(out, "%lf ", it->first[i]);
            }

            vec = it->second;

            std::sort(vec->begin(), vec->end(), trlt);


            double numResults;
            if(targetTrial == -1) {
                numResults = vec->at(0)->getAvgTrialNumResults();
            }
            else {
                numResults = vec->at(0)->getTrialNumResults(targetTrial);
            }

            
            for(vIt = vec->begin(); vIt != vec->end(); vIt++) {

                TimingResult* tr = (*vIt);

                std::cout << "name: " << tr->getIndexName()
                          << " numResults: " << tr->getAvgTrialNumResults()
                          << std::endl;

                //DBUTL_ASSERT((tr->getAvgTrialNumResults() == numResults));

                double ba, qa, ua;
                double baMin, qaMin, uaMin;
                double baMax, qaMax, uaMax;

                if(targetTrial == -1) {
                    ba = tr->getAvgTrialTime(BUILD);
                    qa = tr->getAvgTrialTime(QUERY);
                    ua = tr->getAvgTrialTime(UPDATE);

                    baMin = tr->getMinTrialTime(BUILD);
                    qaMin = tr->getMinTrialTime(QUERY);
                    uaMin = tr->getMinTrialTime(UPDATE);

                    baMax = tr->getMaxTrialTime(BUILD);
                    qaMax = tr->getMaxTrialTime(QUERY);
                    uaMax = tr->getMaxTrialTime(UPDATE);
                }
                else {
                    ba = tr->getTrialTime(targetTrial, BUILD);
                    qa = tr->getTrialTime(targetTrial, QUERY);
                    ua = tr->getTrialTime(targetTrial, UPDATE);
                }


                // printf("minTime build: %lf ", tr->getMinTime(BUILD));
                // printf("maxTime build: %lf ", tr->getMaxTime(BUILD));
                // printf("avgTime build: %lf ", ba);                
                // printf("minTime query: %lf ", tr->getMinTime(QUERY));
                // printf("maxTime query: %lf ", tr->getMaxTime(QUERY));
                // printf("avgTime query: %lf\n", qa);                                
                // printf("minTime update: %lf ", tr->getMinTime(UPDATE));
                // printf("maxTime update: %lf ", tr->getMaxTime(UPDATE));
                // printf("avgTime update: %lf\n", ua);                                

                fprintf(out, "build: %lf query: %lf update: %lf ", ba, qa, ua);

                if(targetTrial == -1) {
                    fprintf(out, "buildMin: %lf queryMin: %lf updateMin: %lf ", 
                            baMin, qaMin, uaMin);
                    fprintf(out, "buildMax: %lf queryMax: %lf updateMax: %lf ", 
                            baMax, qaMax, uaMax);
                }
            }
            fprintf(out, "numResults: %lf\n", numResults);
        }

        for(it = trMap->begin(); it != trMap->end(); it++) {

            std::cout << "[ ";

            for(int i = 0; i < totSize; i++) {
                std::cout << it->first[i] << " ";
            }
            std::cout << "]" << std::endl;

            std::vector<TimingResult*>* vec = it->second;
            for(vIt = vec->begin(); vIt != vec->end(); vIt++) {
                std::cout << "\t" << *((*vIt)->getExperimentRun()) << std::endl;
            }
        }
    } 

    void printAvgBreakdown(FILE* outfile, MapType* trMap, int targetTrial) {

        MapType::iterator it;
        std::vector<TimingResult*>* vec;
        std::vector<TimingResult*>::iterator vIt;
        TimingResult* tr;

        it = trMap->begin();
        vec = it->second;
        TimingResult* first = vec->at(0);


//        int totSize = first->iParams->size() + first->wParams->size();        
        int totSize = first->getNumIndexParams() + first->getNumWorkloadParams();        

        //TODO: Fail silently for the moment. 
        if(totSize > 1) {
            return;
//            throwUsage("Histogram plot unsupported for >1 parameter. ");
        }



        for(it = trMap->begin(); it != trMap->end(); it++) {

            std::cout << "[ ";

            for(int i = 0; i < totSize; i++) {
                std::cout << it->first[i] << " ";
            }
            std::cout << "]" << std::endl;

            std::vector<TimingResult*>* vec = it->second;
            for(vIt = vec->begin(); vIt != vec->end(); vIt++) {
                std::cout << "\t" << *((*vIt)->getExperimentRun()) << std::endl;
            }
        }


        fprintf(outfile, "#Index ");
        for(it = trMap->begin(); it != trMap->end(); it++) {
            fprintf(outfile, "%lf ", it->first[0]);
            vec = it->second;
            std::sort(vec->begin(), vec->end(), trlt);
        }

        fprintf(outfile, "\n");
        int numIndices = vec->size();
        int count = 0;

        double ba, qa, ua;

        for(int i = 0; i < numIndices; i++) {
            if(trMap->begin()->second->size() > i) {
                fprintf(outfile, "%s ", 
                        trMap->begin()->second->at(i)->getIndexName());
                
                for(it = trMap->begin(); it != trMap->end(); it++) {
                    tr = it->second->at(i);



                    if(targetTrial == -1) {
                        ba = tr->getAvgTrialTime(BUILD);
                        qa = tr->getAvgTrialTime(QUERY);
                        ua = tr->getAvgTrialTime(UPDATE);
                    }
                    else {
                        ba = tr->getTrialTime(targetTrial, BUILD);
                        qa = tr->getTrialTime(targetTrial, QUERY);
                        ua = tr->getTrialTime(targetTrial, UPDATE);
                    }



                    fprintf(outfile, "build: %lf query: %lf update: %lf ",
                            ba, qa, ua);
                }
                fprintf(outfile, "\n");            
            }
        }
    } 

    void printTickByTick(FILE* outfile, TimingResult* tr, int targetTrial) {

        double ba, qa, ua, nr; 


        for(int i = 0; i < tr->getNumTicks(); i++) {

            if(targetTrial == -1) {
                ba = tr->getAvgTickTime(i, BUILD); 
                qa = tr->getAvgTickTime(i, QUERY); 
                ua = tr->getAvgTickTime(i, UPDATE); 
                nr = tr->getAvgTickNumResults(i);
            }
            else {
                ba = tr->getTime(targetTrial, i, BUILD); 
                qa = tr->getTime(targetTrial, i, QUERY); 
                ua = tr->getTime(targetTrial, i, UPDATE); 
                nr = tr->getNumResults(targetTrial, i);
            }
            


            fprintf(outfile, 
                    "%d build: %lf query: %lf update: %lf numResults: %lf\n",
                    i, ba, qa, ua, nr);
        }



    }


    enum PlotMode { AVG, AVG_BREAKDOWN, TICK_BY_TICK };
    const char* plotNames[] = {"Avg", "Breakdown", "Tick"};

}


int main(int argc, char *argv[]) {
    
    Log::open("-"); //for asserts;

    bool dirSet = false;
    bool outfileSet = false;
    const char* outfileName;
    //const char* outfileBreakdownName;
    const char* dirName;
    char tmpName[200];
    PlotMode mode;


    const char* indexName;
    const char* tickTrace;
    bool indexNameSet = false;
    bool tickTraceSet = false;
    double* targetParams;
    bool targetParamsSet = false;
    int numTargetParams;
    int targetTrial = -1;

//    double paramVal;
//    bool paramValSet = false;

    std::ostream* traceStream = 0;

    
    const char* modeString = argv[1];

    if(strcmp(modeString, "avg") == 0) {
        mode = AVG;
    }
    else if(strcmp(modeString, "breakdown") == 0) {
        mode = AVG_BREAKDOWN;
    }
    else if(strcmp(modeString, "tick") == 0) {
        mode = TICK_BY_TICK;
    }


    for(int argNum = 2; argNum < argc; argNum++) {
        char const * arg = argv[argNum];
        char const * v;
        std::cout << "arg: " << arg << std::endl;
        if((v = suffixAfterMatch(arg, "--log=")) != 0) {
            IndexLog::setLevel(IndexLog::strToLevel(v));
        }
        else if((v = suffixAfterMatch(arg, "--log-file=")) != 0) {
            if(traceStream == 0) {
                traceStream = new std::ofstream(v);
            }
            IndexLog::setStream(*traceStream);
        }
        // When experiment is set, use default values for dirName and
        // outfileName, unless they are otherwise set.
        else if((v = suffixAfterMatch(arg, "--experiment=")) != 0) {
            if(!dirSet) {
                sprintf(tmpName, "out/traces/%s/", v);
                dirName = tmpName;
                dirSet = true;
            }
            // sowell: Could set outfile here as well, but we have to compute
            // they experiment name and outfile later, so we will do it there. 
            //if(!outfileSet) {
                //outfileName = v;
            //outfileSet = true;
            //}
        }
        else if((v = suffixAfterMatch(arg, "--dir=")) != 0) {
            dirName = v;
            dirSet = true;
        }
        else if((v = suffixAfterMatch(arg, "--outfile=")) != 0) {
            outfileSet = true;
            outfileName = v;
        }
        else if((v = suffixAfterMatch(arg, "--indexName=")) != 0) {
            indexName = v;
            indexNameSet = true;
        }
        else if((v = suffixAfterMatch(arg, "--trial=")) != 0) {
            targetTrial = strtol(v,0,0);
        }
        else if((v = suffixAfterMatch(arg, "--targetParams=")) != 0) {
            std::vector<double> paramsVec;
            const char* pch;
            char v2[50];
            strcpy(v2, v);

            //std::cout << "v2: " << v2 << std::endl;
            pch = strtok(v2, ", ");

            while(pch != NULL){
                //std::cout << "pch: " << pch << std::endl;
                paramsVec.push_back(strtod(pch, 0));
                pch = strtok(NULL, ", ");
            }

            targetParams = new double[paramsVec.size()];
            for(int i = 0; i < paramsVec.size(); i++) {
                //std::cout << "i: " << paramsVec[i] << " ";
                targetParams[i] = paramsVec[i];
            }
            numTargetParams = paramsVec.size();
            targetParamsSet = true;
        }

        else if((v = suffixAfterMatch(arg, "--ticktrace=")) != 0) {
            tickTrace = v;
            tickTraceSet = true;
        }

        //else if((v = suffixAfterMatch(arg, "--paramVal=")) != 0) {
        //paramVal = strtod(v, 0);            
        //paramValSet = true;
        //}
        else {
            throwUsage("Unkown argument to plot.exec.");
        }
    }


    // sowellb tmp 9/27/11
    if(mode == TICK_BY_TICK) {
        if(!outfileSet || !tickTraceSet) {
            throwUsage("Outfile and tickTrace must be set.");
        }


        FILE* outfile = fopen(outfileName, "w"); 

        TimingResult* tr = TimingResult::deserialize(tickTrace);
        printTickByTick(outfile, tr ,-1);
        
        return 0; 

    }



    if(!dirSet) {
        throwUsage("Directory must be set using --dir=PATH.");
    }

    std::cout << "dirName: " << dirName << std::endl;


    char experName[200];
//    std::vector<const char*>* iParams = NULL; 
//    std::vector<const char*>* wParams = NULL;

    std::cout << "before createMap: " << std::endl;
    MapType* trMap = createMap(dirName, experName/*, iParams, wParams*/);

    char tmpOutfile[200];

    if(outfileSet == false) {
        if(targetTrial == -1) {
            sprintf(tmpOutfile, "out/outfiles/%s%s.txt", 
                    experName, plotNames[(int) mode]);
        }
        else {
            sprintf(tmpOutfile, "out/outfiles/%sTrial%d%s.txt", 
                    experName, targetTrial, plotNames[(int) mode]);
        }

        outfileName = tmpOutfile;
    }

    FILE* outfile = fopen(outfileName, "w");

    if(outfile == 0) {
         throwUsage("Couldn't open outfile.");
    }

//    std::vector<TimingResult*>* vec;
    std::vector<TimingResult*>::iterator vIt;


    // FILE* outfileBreakdown = fopen(outfileBreakdownName, "w");
    // if(outfileBreakdown == 0) {
    //     throwUsage("Couldn't open breakdown outfile.");
    // }
        

    
    //std::map<double, std::vector<TimingResult*>*>::iterator it;


    std::cout << "outfile: " << outfileName << std::endl;

     MapType::iterator it;
    // for(it = trMap->begin(); it != trMap->end(); it++) {

    //     std::cout << "[ ";

   //     for(int i = 0; i < iParams->size() + wParams->size(); i++) {
    //         std::cout << it->first[i] << " ";
    //     }
    //     std::cout << "]" << std::endl;

    //     std::vector<TimingResult*>* vec = it->second;
    //     for(vIt = vec->begin(); vIt != vec->end(); vIt++) {
    //         std::cout << "\t" << (*vIt)->getIndexName() << std::endl;
    //     }
    // }

    if(mode == AVG) {
        printAvgFile(outfile, trMap, targetTrial);
    }
    else if(mode == AVG_BREAKDOWN) {
        printAvgBreakdown(outfile, trMap, targetTrial);
    }

    //TODO: Support multiple indices in a single graph. 
    else if(mode == TICK_BY_TICK) {

        if(!targetParamsSet) {
            throwUsage("targetParams must be set in tick mode.");
        }
        if(!indexNameSet) {
            throwUsage("indexName must (currently) be set in tick mode.");
        }
        if(trMap->count(targetParams) != numTargetParams) {
            throwUsage("Unknown targetParams in tick mode.");
        }

        std::vector<TimingResult*>* vec = (*trMap)[targetParams];
        
        for(int i = 0; i < vec->size(); i++) {
            TimingResult* tr = vec->at(i);
            std::cout << tr->getIndexName() << std::endl;

            if(strcmp(tr->getIndexName(), indexName) == 0) {
                std::cout << "printing." << std::endl;
                printTickByTick(outfile,tr,targetTrial);
            }
        }
            

    }

    if(targetParamsSet)
        delete [] targetParams;

    std::vector<TimingResult*>* vec;
    for(it = trMap->begin(); it != trMap->end(); it++) {
        delete [] it->first;
        vec = it->second;
        for(int i = 0; i < vec->size(); i++) {
            delete vec->at(i);
        }
        delete vec;
    }

    delete trMap;
}

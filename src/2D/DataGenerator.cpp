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

#include "2D/DataGenerator.h"
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <iostream>


using namespace PSimIndex;

const char* DataGenerator::testParam(const char *s, const char *flagArg) {
    size_t n = strlen(flagArg);
    char const * ans = ((strncmp(s, flagArg, n) == 0) ? (s+n) : 0);
    return ans;
}

 void DataGenerator::openTrace() {
     if(!outfileSet) {
         throw std::logic_error("DataGenerator: No trace file set.");
     }
     writer = new PSimIndex::TraceWriter2D(outfile, binary);
 }

 void DataGenerator::closeTrace() {
     delete writer;
 }


 void DataGenerator::setParam(const char* param) {

     const char* v;
     if((v = testParam(param, "--outfile=")) != 0) {
         outfile = v;
         outfileSet = true;
     }
     else if( (v = testParam(param, "--binary=")) != 0) {
         if(strcmp(v, "true") == 0 || strcmp(v, "True") == 0) {
             setBinary(true);
         }
         else if(strcmp(v, "false") == 0 || strcmp(v, "False") == 0) {
             setBinary(false);
         }
         else {
             throw std::logic_error("--binary takes true or false.");
         }
     }
     else if((v = testParam(param, "--numPoints=")) != 0) {
         setNumPoints(strtol(v, 0, 0));
     }
     else if((v = testParam(param, "--numTicks=")) != 0) {
         setNumTicks(strtol(v, 0, 0));
     }
     else if((v = testParam(param, "--seed=")) != 0) {
         const char* pch;
         char v2[50];
         strcpy(v2, v);

         pch = strtok(v2, " ,");

         if(pch == 0)
             throw std::logic_error("DataGenerator: Unexpected seed format.");

         int seed1 = strtol(pch, 0, 0);

         pch = strtok(NULL, " ,");
         if(pch == 0)
             throw std::logic_error("DataGenerator: Unexpected seed format.");

         int seed2 = strtol(pch, 0, 0);
         setSeed(seed1, seed2);
     }
     else {
         throw std::logic_error("DataGenerator: Unknown parameter.");
     }

 }


 DataGenerator::DataGenerator() {
     binary = false;
     numPoints = 100000;
     numTicks = 500;
     rng = new RNG();
 }


 DataGenerator::~DataGenerator() {
//     delete writer;
     delete rng;
}

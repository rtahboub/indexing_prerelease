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
#include <cstring>
#include <cstdlib>
#include <stdexcept>

#include "2D/data_generator/UniformGenerator.h"
#include "2D/data_generator/GaussianGenerator.h"
#include "2D/data_generator/NetworkGenerator.h"
#include "2D/data_generator/FishGenerator.h"
#include "2D/point_index/RTree.h"
#include "2D/point_index/TPRTree.h"
#include "2D/join_index/SynchTraversal.h"
#include "2D/TraceWriter2D.h"
#include "IndexLog.h"

using namespace PSimIndex;

namespace /*anon*/ {
    // Taken from Al's code. Should use DBUtl/CmdLine, but the logging is
    // wrong...
    char const * suffixAfterMatch(const char *s, const char *flagArg) {
        size_t n = strlen(flagArg);
        char const * ans = ((strncmp(s, flagArg, n) == 0) ? (s+n) : 0);
        return ans;
    }

    enum DataGenMode { UNIFORM, GAUSSIAN, FISH, NETWORK, CONVERT, PREFIX };



    bool isBinary(const char* filename) {
        if(strstr(filename, ".dat") != 0) {
            return true;
        }
        else if(strstr(filename, ".txt") != 0) {
            return false;
        }
        else {
             throw std::logic_error("Unkown workload file extension. \
                                    Should be .dat or .txt");
        }
    }
}

int main(int argc, char *argv[]) {

    DataGenerator* dg;

    DataGenMode mode;
    const char* tracefile;
    const char* outfile;
    bool outfileSet = false;
    bool tracefileSet = false;
    std::ostream* traceStream = 0;


    int numTicks = -1;

    IndexLog::setLevel(INFO);

    if(argc < 2) {
        throw std::logic_error("usage: ./datagen mode [args...]");
    }

    const char* modeString = argv[1];

    if(strcmp(modeString, "uniform") == 0) {
//        dg = new RandomGenerator();
        dg = new UniformGenerator();
        mode = UNIFORM;
    }
    else if(strcmp(modeString, "gaussian") == 0) {
        dg = new GaussianGenerator();
        mode = GAUSSIAN;
    }
    else if(strcmp(modeString, "fish") == 0) {
        dg = FishGenerator::createFishGenerator<TPRTreeDefault>();
        mode = FISH;
    }
    else if(strcmp(modeString, "network") == 0) {
        dg = new NetworkGenerator();
        mode = NETWORK;
    }
    else if(strcmp(modeString, "convert") == 0) {
        mode = CONVERT;
    }
    else if(strcmp(modeString, "prefix") == 0) {
        mode = PREFIX;
    }
    else {
        throw std::logic_error("Unexpected mode");
    }

        
    for(int argNum = 2; argNum < argc; argNum++) {
        char const * arg = argv[argNum];
        char const * v;

        if((v = suffixAfterMatch(arg, "--log=")) != 0) {
            IndexLog::setLevel(IndexLog::strToLevel(v));
        }
        else if((v = suffixAfterMatch(arg, "--log-file=")) != 0) {
            if(traceStream == 0) {
                traceStream = new std::ofstream(v);
            }
            IndexLog::setStream(*traceStream);
        }
        else if(mode == CONVERT || mode == PREFIX) {
            if((v = suffixAfterMatch(arg, "--outfile=")) != 0) {
                outfile = v;
                outfileSet = true;
            }
            else if((v = suffixAfterMatch(arg, "--trace=")) != 0) {
                tracefile = v;
                tracefileSet = true;
            }
            else if((v = suffixAfterMatch(arg, "--numTicks=")) != 0) {
                numTicks = strtol(v, 0, 0);
            }
            else {
                throw std::logic_error("Unknown convert argument");
            }
        }
        else {

            dg->setParam(arg);
        }
    }


    if(mode == CONVERT) {
        if(!tracefileSet || !outfileSet) {
            throw std::logic_error("ERROR: Tracefile and outfile must \
                                    be specified.");
        }
        if(isBinary(tracefile)) {
            TraceWriter2D::binaryToText(tracefile, outfile);
        }
        else {
            TraceWriter2D::textToBinary(tracefile, outfile);
        }
    }
    else if(mode == PREFIX) {
        if(!tracefileSet || !outfileSet) {
            throw std::logic_error("ERROR: Tracefile and outfile must \
                                    be specified.");
        }
        if(numTicks == -1) {
            throw std::logic_error("ERROR: numTicks must be specified \
                                    in prefix mode. ");
        }

        //TraceWriter2D::createPrefixFile(tracefile, outfile, isBinary(tracefile), numTicks);
    }
    else {
        dg->genTrace();
    }

    return 0;
}


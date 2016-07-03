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

#ifndef _DATA_GENERATOR_H_
#define _DATA_GENERATOR_H_

#include "2D/TraceWriter2D.h"
#include "util/RNG.h"

namespace PSimIndex {

    class DataGenerator {
    protected:
        TraceWriter2D* writer;
        const char* outfile;
        bool binary, outfileSet;
        int numPoints, numTicks;
        RNG* rng;


        // Taken from Al's code. Should use DBUtl/CmdLine, but the logging is
        // wrong...
        char const * testParam(const char *s, const char *flagArg);
        void openTrace();
        void closeTrace();


    public:

        inline void setOutfile(const char* outfile) {
            outfileSet = true;
            this->outfile = outfile;
        }


        inline void setBinary(bool binary){ this->binary = binary; }
        inline void setNumPoints(int numPoints){ this->numPoints = numPoints; }
        inline void setNumTicks(int numTicks){ this->numTicks = numTicks; }
         inline void setSeed(int seed1, int seed2) {
            delete rng;
            rng = new RNG(seed1, seed2);
        }



        virtual void setParam(const char* param);
        virtual void genTrace() = 0;

        DataGenerator();
        virtual ~DataGenerator();
    };
}

#endif /* _DATA_GENERATOR_H_ */

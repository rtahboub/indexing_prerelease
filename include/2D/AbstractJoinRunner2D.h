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

#ifndef _ABSTRACT_JOIN_RUNNER2D_H_
#define _ABSTRACT_JOIN_RUNNER2D_H_

#include "2D/TestTypes2D.h"
#include "TimingResult.h"

namespace PSimIndex {

    /**
     * Abstract Base class for StaticJoinRunner2D and
     * IncrementalJoinRunner2D. This was originally folded into JoinRunner2D,
     * but this led to a circular dependency. 
     *
     * TODO: I've removed support for multiple queries/tick in order to allow
     * the caller to pass in a callback. Is there a way to have both??
     */

    class AbstractJoinRunner2D {

    public:
        virtual bool hasNextTick() = 0;
        virtual void runTick(JoinCallBack<Key2D,Point2D,Point2D>* cb) = 0;
        //virtual std::vector<JoinCallBack<Key2D,Point2D,Point2D>*>* getCallBacks() = 0;
        //virtual void clearCallBacks() = 0;
        virtual TimingResult* getTimingResult() = 0;
        virtual ~AbstractJoinRunner2D(){}
    };





}



#endif /* _ABSTRACT_JOIN_RUNNER2D_H_ */

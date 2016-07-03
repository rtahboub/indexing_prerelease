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

/*
 *  AggValues.h
 *  TemplateTests
 *
 *  Created by Alan Demers on 5/12/09.
 *  Copyright 2009 Cornell. All rights reserved.
 *
 */

#ifndef _AGG_VALUES_H_
#define _AGG_VALUES_H_


template <typename U>
struct NullAggValues {
    inline void reset() {};
    inline void combine(NullAggValues *) {}
    inline void combineFromOne(U *) {}
    NullAggValues() {}
    ~NullAggValues() {}
};


template <typename U>
struct CountAggValues {
    unsigned count;
    inline void reset() { count = 0; }
    inline void combine(CountAggValues * p) { count += p->count; }
    inline void combineFromOne(U *) { count += 1; }
    CountAggValues() : count(0) {}
    ~CountAggValues() {}
};


#endif /* _AGG_VALUES_H_ */

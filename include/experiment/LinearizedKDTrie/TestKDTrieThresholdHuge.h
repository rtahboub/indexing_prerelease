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

#ifndef _TEST_KD_TRIE_THRESHOLD_HUGE_H_
#define _TEST_KD_TRIE_THRESHOLD_HUGE_H_

#include "2D/Experiment2D.h"
#include "TimingResult.h"
#include "2D/point_index/LinearizedKDTrie.h"
#include "2D/workload/TraceWorkload2D.h"
#include "2D/TestTypes2D.h"

namespace PSimIndex {
    class TestKDTrieThresholdHuge : public Experiment2D {
    public:
        void init() {

            addIndexParam("Threshold");
            folder = "out/traces/LinearizedKDTrie/TestKDTrieThresholdHuge/";
            experimentName = "TestKDTrieThresholdHuge";
            
//            for(double i = 0.0; i < 1.0; i += 0.1) {
            for(double i = 0.9; i >= 0.0; i -= 0.1) {
                addIndex(getJoinMethod(new LinearizedKDTrie<Point2D, Key2D, SumAgg2D>(i)), i);
            }

            addWorkload(new TraceWorkload2D("traces/hugetest.dat", true), 0);

        }

        TestKDTrieThresholdHuge() : Experiment2D() {}
    };
}

#endif /* _TEST_KD_TRIE_THRESHOLD_HUGE_H_ */

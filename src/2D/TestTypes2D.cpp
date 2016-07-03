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

#include "2D/TestTypes2D.h"
#include "2D/join_index/StaticNestedLoops2D.h"
#include "2D/join_index/MovingNestedLoops2D.h"
#include <iostream>
#include <iomanip>


using namespace PSimIndex;
using namespace std;

int Point2D::nextID = 0;


JoinMethod* PSimIndex::getJoinMethod(IndexType* index) {
    JoinType* j;
    if(index->isMovingIndex()) {
        MovingSpatialIndex2D<Point2D,Key2D,SumAgg2D>* mv;
        mv = (MovingSpatialIndex2D<Point2D,Key2D,SumAgg2D>*)(index);
        j = new MovingNestedLoops2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(mv);
    }
    else {
    	//std::cout <<"getJoinMethod, static\n";
        StaticSpatialIndex2D<Point2D,Key2D,SumAgg2D>* st;
        st = (StaticSpatialIndex2D<Point2D,Key2D,SumAgg2D>*)(index);
        j = new StaticNestedLoops2D<Point2D,Key2D,SumAgg2D,RegionGen2D>(st);
    }
    return new JoinMethod(j);
}

JoinMethod* PSimIndex::getJoinMethod(JoinType* j) {
    return new JoinMethod(j);    
}

JoinMethod* PSimIndex::getJoinMethod(ContJoinType* cj) {
    return new JoinMethod(cj);    
} 

// template<typename A, typename B>
// CallBackMethod<A,B>* PSimIndex::getCallBackMethod(JoinCallBack<A,B>* jcb) {

// }

// template<typename A, typename B>
// CallBackMethod<A,B>* PSimIndex::getCallBackMethod(IncrementalCallBack<A,B>* icb) {
//     return new CallBackMethod<A,B>(icb);
// }


std::ostream& PSimIndex::operator<<(std::ostream &out, 
                                    const PSimIndex::Point2D& p) {
    out << "Point2D(" << setiosflags(ios::fixed) << setprecision(2) 
        << p.id << ", " << p.x << ", " << p.y << ", " 
        << p.vx << ", " << p.vy << ", " << p.data << ")" ;
    return out;
}



std::ostream& PSimIndex::operator<<(std::ostream &out, 
                                    const PSimIndex::RegionGen2D& p) {
    out << "RegionGen2D(" << p.xDelta << ", " << p.yDelta << ")";
    return out;
}


std::ostream& PSimIndex::operator<<(std::ostream &out, 
                                    const PSimIndex::SumAgg2D& s) {
    return out << "SumAgg2D(" << s.getVal() << ")";
}

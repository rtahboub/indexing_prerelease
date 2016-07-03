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

#include "2D/Workload2D.h"
#include "2D/TestTypes2D.h"
#include "2D/Region2D.h"
#include <iostream>

using namespace PSimIndex;

std::ostream& PSimIndex::operator<<(std::ostream &out, const QueryRec& q) {
    out << "QueryRec{";
    switch(q.type) {
    case ENUM:
        out << "ENUM, " << "region = " << *q.regionGen;
        break;
    case AGG:
        out << "AGG, " << "region = " << *q.regionGen;
        break;
    case KNN:
        out << "KNN, k = " << q.k;
        break;
    }

    out << ", num queriers = ";
    if(q.queryPoints == 0) {
        out << "0 }";
    }
    else {
        out << q.queryPoints->size() << "}";
    }


    return out ;
}

std::ostream& PSimIndex::operator<<(std::ostream &out, const UpdateRec& u) {
    out << "UpdateRec{";

    switch(u.type) {
    case VEL_UPDATE:
        out << "VEL_UPDATE, ";
        out << "newVx = " << u.newVx << " newVy = " << u.newVy << ", ";
        break;
    case POS_UPDATE:
        out << "POS_UPDATE, ";
        out << "newX = " << u.newX << " newY = " << u.newY << ", ";
        break;
    case AGG_UPDATE:
        out << "AGG_UPDATE, ";
        break;
    case INSERT:
        out << "INSERT, ";
        break;
    case DELETE:
        out << "DELETE, ";
//        out << "newX = " << u.newX << " newY = " << u.newY 
//            << " newVx = " << u.newVx << " newVy = " << u.newVy << ", ";
        break;
    case IMPLICIT_MOVE:
        out << "IMPLICIT_MOVE, ";        
        break;

    }
//    return out;
    return out << " point = " << *(u.point) << " " << u.point << "}";
}

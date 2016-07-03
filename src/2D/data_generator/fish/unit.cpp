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

#include "2D/data_generator/fish/unit.h"
#include <iostream>
#include <iomanip>

using namespace PSimIndex;
using namespace std;

// constant initialization
const float Unit::ALPHA = 25.0f;
const float Unit::RHO = 160.0f;
const float Unit::GAUSS_MU = 0.0f;
const float Unit::GAUSS_SIGMA = 0.01f;
const float Unit::THETA = 0.08f;
const float Unit::P = 0.15f;
const float Unit::OMEGA = 0.5f;

std::ostream& PSimIndex::operator<<(std::ostream &out, 
                                    const PSimIndex::Unit& p) {
    out << "Unit(" << setiosflags(ios::fixed) << setprecision(2) 
        << p.getOid() << ", " << p.getX() << ", " << p.getY() << ", " 
        << p.getVx() << ", " << p.getVy() << ")" ;
    return out;
}

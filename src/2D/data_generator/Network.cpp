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
#include "2D/data_generator/Network.h"

std::ostream& PSimIndex::operator<<(std::ostream &out, 
                                    const PSimIndex::Node& n) {
    out << "Node(" << n.nid << ", " << n.x << ", " << n.y << ", " << n.degree;
    out << " [";
    for(int i = 0; i < n.degree; i++) {
        out << n.edges[i]->eid << " ";
    }
    out << "])";
    return out;
}


std::ostream& PSimIndex::operator<<(std::ostream &out, 
                                    const PSimIndex::Edge& e) {
    return out << "Edge(" << e.eid << ", " << e.s->nid << ", " 
               << e.e->nid << ", " << e.length << ")";
}


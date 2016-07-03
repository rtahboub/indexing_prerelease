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

#include "SimpleHash.h"

using namespace PSimIndex;



const int SimpleHash::DEFAULT_TABLE_SIZE = 100;

void SimpleHash::insert(Point2D* key) {
    // std::cout << "In insert: " << *key << " k = " << (key->id % tableSize) << std::endl;
    // std::cout << "mySize: " << mySize << std::endl;
    //vector<Point2D*>::iterator it;
    mySize++;
    //std::vector<Point2D*>* vec = vArray[(key->id % tableSize)];

    int k = (key->id % tableSize);

    Bucket* b = &(vArray[k]);

    if(!setArray[k]) {
        b->data = key;
        b->next = NULL;
        setArray[k] = true;
    }
    else {
        Bucket* prev = b;

        // std::cout << "Bucket b data "<< *b->data << std::endl;

        while(b != NULL) {
            // std::cout << "b->data" << *b->data << std::endl;
            if(b->data->id == key->id) {
                // std::cout << "in if" << std::endl;
                b->data = key;
                return;
            }
            prev = b;
            b = b->next;

        } 
        prev->next = new Bucket(key);
        // std::cout << "allocating ";
        // if(prev->next->next != NULL) {
        //     std::cout << " NOT NULL" << std::endl;
        // }
        // else {
        //     std::cout << " NULL" << std::endl;
        // }
    }
//    std::cout << "After loops" << std::endl;
    // for(int i = 0; i < vec->size(); i++) {
    //     if(key->id == vec->at(i)->id) {
    //         (*vec)[i] = key;
    //         return;
    //     }
    // }
    // vec->push_back(key);
}



void SimpleHash::erase(Point2D* key) {

    // std::cout << "in erase" << std::endl;

    int k = (key->id % tableSize);

    Bucket* b = &vArray[k];
    Bucket* prev = b;

    if(!setArray[k]) {
        return;
    }
    //Head matches
    else if(key->id == b->data->id) {
        Bucket* b2 = b->next;
        b->data = b2->data;
        b->next = b2->next;        
        delete b2;
        mySize--;
        return;
    }
    
    b = b->next;

    while(b != NULL) {
        if(key->id == b->data->id) {
            if(b->next != NULL) {
                prev->next = b->next;
            }
            else {
                prev->next = NULL;
            }
            delete b;
            mySize--;
            return;
        }
        prev = b;
        b = b->next;
    }



    // std::vector<Point2D*>* vec = vArray[(key->id % tableSize)];
    // for(int i = 0; i < vec->size(); i++) {
    //     if(vec->at(i)->id == key->id) {
    //         //Point2D* tmp = vec->at(i);
    //         (*vec)[i] = vec->at(vec->size() - 1);
    //         vec->pop_back();
    //         mySize--;
    //         return;
    //     }
    // }

}


bool SimpleHash::contains(Point2D* key) {
    int k = (key->id % tableSize);
    Bucket* b = &vArray[k];
    //Bucket* prev = b;
    if(!setArray[k]) {
        return false;
    }


    while(b != NULL) {
        if(key->id == b->data->id) {
            return true;
        }
        b = b->next;
    }
    return false;
    // std::vector<Point2D*>* vec = vArray[(key->id % tableSize)];
    // for(int i = 0; i < vec->size(); i++) {
    //     if(key->id == vec->at(i)->id) {
    //         return true;
    //     }
    // }
    // return false;
}


int SimpleHash::size() {
    return mySize;
}

SimpleHash::SimpleHash() {
//    std::cout << "in SimpleHash" << std::endl;

    tableSize = DEFAULT_TABLE_SIZE;

    setArray = (bool*) calloc(sizeof(bool), tableSize);

    vArray = new Bucket[tableSize];


    for(int i = 0; i < tableSize; i++) {
        if(vArray[i].next != NULL) {
            std::cout << "NOT NULL!!!" << std::endl;
        }
    }

//vArray = new std::vector<Point2D*>*[tableSize];

    // for(int i = 0; i < tableSize; i++) {
    //     vArray[i] = new std::vector<Point2D*>();
    // }

    mySize = 0;
}

void SimpleHash::clear() {
    //std::cout << "in clear" << std::endl;
    
    double avgSize = 0;
    int maxSize = -1;
    int minSize = std::numeric_limits<int>::max();

    for(int i = 0; i < tableSize; i++) {
        Bucket* b = &vArray[i];

        // int vSize = vArray[i]->size();

        // if(vSize > maxSize) {
        //     maxSize = vSize;
        // }
        // if(vSize < minSize) {
        //     minSize = vSize;
        // }
        // avgSize += vSize;


        while(b->next != NULL) {
            Bucket* b2 = b->next;
            b->next = b2->next;
            delete b2;
        }



        //vArray[i]->clear();
    }
    
    memset(setArray, 0, tableSize * sizeof(bool));

    //std::cout << "Avg Size / Bucket: " << (avgSize / tableSize) << " minSize: " << minSize
    //<< " maxSize: "  << maxSize << std::endl;
    mySize = 0;
}

SimpleHash::~SimpleHash() {
//    std::cout << "in destructor" << std::endl;
    clear();
    // for(int i = 0; i < tableSize; i++) {
    //     delete vArray[i];
    // }

    delete [] vArray;
}

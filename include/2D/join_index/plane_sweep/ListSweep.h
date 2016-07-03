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

#ifndef _LIST_SWEEP_H_
#define _LIST_SWEEP_H_

#include "2D/join_index/plane_sweep/SweepStructure.h"

#include <iostream>

namespace PSimIndex {

    template <typename U, typename K>
    class ListSweep : public SweepStructure<U,K> {

    private:

        typedef Region2D<typename K::KeyType0, typename K::KeyType1> RType;

        struct ListNode {
            ListNode *next, *prev;
            RType query;
            U* source;

            ListNode(RType query, U* source) 
                : query(query), source(source), next(NULL), prev(NULL)
            {}
        };
            
        ListNode* head;
        ListNode* tail;

        bool toDel;
        U* deleteLimit;

        int size;

    public:

        const char* getName() {
            return "List";
        }

        void insert(RType r, U* source) {
            ListNode* newNode = new ListNode(r, source);

            if(head == NULL) {
                head = tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
            size++;
        }

        void print() { 
            ListNode* curr = head;            
            std::cout << "List:" << std::endl;
            std::cout << "--------------" << std::endl;
            while(curr != NULL) {
                std::cout << curr->query << " " << *(curr->source) << std::endl;
                curr = curr->next;
            }
            std::cout << "--------------" << std::endl;
        }

        void removeExpired(U* limit) {
            toDel = true;
            deleteLimit = limit;

        }




        void reportOverlap(JoinCallBack<K,U,U>* cb, U* point) {

            ListNode* curr = head;
            ListNode* prev = NULL;

            while(curr != NULL) {
                //Remove expired rectangles. 
                if(toDel && curr->query.getHigh0() < K::getKey0(deleteLimit)){
                    if(curr == head) {
                        head = curr->next;
                        delete curr;
                        curr = head;

                    }
                    else if(curr == tail) {
                        tail = prev;
                        tail->next = NULL;
                        delete curr;
                        curr = NULL;
                    }
                    else {
                        prev->next = curr->next;
                        delete curr;
                        curr = prev->next;
                    }
                    size--;
                    continue;
                }
                else if(curr->query.template containsPoint<U,K>(point) 
                        && K::getOID(curr->source) != K::getOID(point)) {
                    cb->reportPair(curr->source, point);
                }
                prev = curr;
                curr = curr->next;
            }
            toDel = false;
        }

        virtual void setMinMax(typename K::KeyType1 minY, 
                               typename K::KeyType1 maxY) {}
        

        /**
         * Not always accurate b/c deletes are lazy. 
         */
        int getSize() {
//            processDelete();
            return size;
        }

        void clear() {
            ListNode* curr = head;
            ListNode* tmp;
         
            while(curr != NULL) {
                tmp = curr;
                curr = curr->next;
                delete tmp;
            }
            head = tail = NULL;
            size = 0;

        }

        ListSweep() {
            toDel = false;
            head = tail = NULL;
            size = 0;
        }

        ~ListSweep() {
            clear();
        }

    };
}



#endif /* _LIST_SWEEP_H_ */


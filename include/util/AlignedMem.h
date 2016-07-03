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

#ifndef _DBUTL_ALIGNED_MEM_H_
#define _DBUTL_ALIGNED_MEM_H_

/*
 *  AlignedMem.h
 *  DBUtl
 *
 *  Created by Alan Demers on 10/10/09.
 *  Copyright 2009 Cornell. All rights reserved.
 *
 */
 
#include <cstdlib> /* for size_t */

class AlignedMem {

public:
	static size_t const DEFAULT_ALIGN = sizeof(double);
	static void * allocate(size_t nBytes, size_t alignSize=DEFAULT_ALIGN);
	static void * allocateClear(size_t nBytes, size_t alignSize=DEFAULT_ALIGN);
	static size_t getSize(void * p);
	static void deallocate(void * p);
};

#endif /* _DBUTL_ALIGNED_MEM_H_ */

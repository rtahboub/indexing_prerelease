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

#include "util/AlignedMem.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>

struct AlignHdr {
	void * _b;
	size_t _n;
};

void * AlignedMem::allocate(size_t nBytes, size_t alignSize) {
#	ifndef DBUTL_ALIGNED_MEM_NDEBUG
		if( (alignSize & (alignSize-1)) != 0 ) {
			throw std::logic_error(
				"AllignedMem::allocate alignment not a power of 2" ); 
		}
#	endif
	void * base = malloc(nBytes + alignSize + sizeof(AlignHdr));
	char * p = (char *)base + sizeof(AlignHdr);
	size_t off = ((size_t)(p)) & (alignSize-1);
	p = p - off + alignSize;
	((AlignHdr *)(p))[-1]._b = base;
	((AlignHdr *)(p))[-1]._n = nBytes;
	return (void *)(p);
}

void * AlignedMem::allocateClear(size_t nBytes, size_t alignSize) {
	void * p = allocate(nBytes, alignSize);
	memset( p, 0, nBytes );
	return (void *)(p);
}

size_t AlignedMem::getSize(void * p) {
	size_t n = ((AlignHdr *)(p))[-1]._n;
	return n;
}

void AlignedMem::deallocate(void * p) {
	void * base = ((AlignHdr *)(p))[-1]._b;
	free( base );
}

namespace /* anon */ {
	/* force the compiler to instantiate allocate */
	void * (*allocatePtr)(size_t, size_t) = &(AlignedMem::allocate);
}

/*
 * TODO: when "make check" is available ...
 */
#ifdef DBUTL_ALIGNED_MEM_TEST
#include <cstdio>
#include <cstdlib>
#include "AlignedMem.h"

#define COUNT 20
size_t ALIGN = 16;

int main( int argc, char const ** argv ) {

	if( argc > 1 ) ALIGN = (size_t)(atoi(argv[1]));

	fprintf(stdout, "Hello, World!\n");
	fprintf(stdout, "Alignment %zu:\n", ALIGN);
	for( size_t s = 8; s <= 4096; s <<= 1 ) {
		void * ptrs[COUNT];
		for( int i = 1; i < COUNT; i++ ) {
			ptrs[i] = AlignedMem::allocate(i*s, ALIGN);
			fprintf(stdout, "\tSize %zd (%zd)  Addr %zx\n", i*s, AlignedMem::getSize(ptrs[i]), (size_t)(ptrs[i]));
		}
		for( int i = COUNT-1; i > 0; i-- ) {
			AlignedMem::deallocate(ptrs[i]);
		}
	}
	fprintf(stdout, "Good-Bye, World!\n");
	return 0;
}
#endif /* DBUTL_ALIGNED_MEM_TEST */

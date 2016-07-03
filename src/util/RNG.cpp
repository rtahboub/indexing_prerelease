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
** RNG.cpp
**
** Random Number Generator
**
** Derived from a simple generator due to lecuyer:
**
**   http://www-cgrl.cs.mcgill.ca/~luc/lecuyer.c
**
** This is posted on the Web; but I (ajd) dont know
** what license applies.
**
** Modified by adj to
**	- be in C++
**	- create independently-seeded random number generator objects
**	- batch the computation for inlining
**
** This works on 64-bit platforms only if (sizeof(int) == 4) and
** double is 64-bit IEEE floating point. 
*/

// TODO: use pstdint.h and declare the sizes of things

#include "util/RNG.h"
#include <ctime>

static char intSizeCheck[1 - 2*(sizeof(int) != 4)];

RNG RNG::_global(55555, 99999);

static inline unsigned altbits(unsigned x) {
    return x & 0x55555555;
}

void RNG::_setseed(int s1, int s2) {
	_s1 = (s1 ? s1 : altbits(time(0)));
	_s2 = (s2 ? s2 : altbits(time(0)>>1));
    _fill(); // TODO: is this necessary?
	_ix = 0;
}


void RNG::_fill() {
  static const double factor = 1.0/2147483563.0;
  unsigned s1 = _s1;
  unsigned s2 = _s2;
  for( int i = _CACHE_SZ - 1; i >= 0; i-- ) {
    { int k1 = s1 / 53668;
	  s1 =40014*(s1%53668)-k1*12211;
	  if (s1 < 0) s1 += 2147483563;
	}
	{ int k2 = s2 / 52774;
	  s2=40692*(s2%52774)-k2*3791;
	  if (s2 < 0) s2 += 2147483399;
	}
	/*
	z = abs(s1 ^ s2);
	*/
	{ int z = (s1 - 2147483563) + s2;
	  if (z < 1) z += 2147483562;
	  _cache[i] = (((double)(z))*factor);
	}
  }
  _s1 = s1;
  _s2 = s2;
  _ix = _CACHE_SZ;
}


/**
 * A generator for the Gaussian distribution. Taken from:
 *    	
 * Jim Gray, Prakash Sundaresan, Susanne Englert, Ken Baclawski, Peter J. Weinberger. 
 * Quickly generating billion-record synthetic databases, SIGMOD 1994
 * (http://portal.acm.org/citation.cfm?id=191886)
 */
double RNG::gauss(double mu, double sigma) {
    double ans = 0.0;
    
    for (int i = 0; i < 12; i++){
        ans = ans + (rand()) - 0.5;
    }
    
    return (mu + sigma*ans/6.0) ;
}


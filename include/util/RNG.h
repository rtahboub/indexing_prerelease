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
** RNG.h
**
** Random Number Generator
**
** Assumes int, unsigned and float are 32 bits, double is 64.
*/
#ifndef _DBUTL_RNG_H_
#define _DBUTL_RNG_H_

class RNG {

private:
	static const unsigned _CACHE_SZ = 20;
	static RNG _global;

	unsigned _ix;
	int _s1;
	int _s2;
	double _cache[_CACHE_SZ];
	
	void _fill();
    void _setseed(int s1, int s2);

public:
	/*
	 * set the global seed
	 */
	static void setseed(int s1, int s2) { _global._setseed(s1, s2); }
	/*
	 * get the global generator, e.g. RNG::get().rand()
	 */
	inline static RNG & get() {
		return _global;
	}
	/*
	 * return random double distributed uniformly in [0, 1)
	 */
	inline double rand() {
		if( _ix == 0 ) _fill();
		return _cache[--(_ix)];
	}
    inline float randf() {
		if( _ix == 0 ) _fill();
		return (float)(_cache[--(_ix)]);
    }
	/*
	 * return true with probability p
	 */
	inline bool flip(double p) {
		return (rand() <= p);
	}
	/*
	 * choose an unsigned distributed uniformly in [0, n)
	 */
	inline unsigned select(unsigned n) {
		return ((unsigned)(n * rand()));
	}
    /*
     * choose an int distributed uniformly in [minValue, maxValue)
     */
    inline int uniformInt(int minValue, int maxValue) {
        return minValue + select(maxValue - minValue);
    }
    /*
     * gaussian
     */
    double gauss(double mu, double sigma);
	/*
	 * construct a new object with given seed pair ...
	 *	default is a random seed.
	 * TODO: (ajd) is there any admissibility test for seed pairs?
	 */
	RNG(int s1 = 0, int s2 = 0) { _setseed(s1, s2); }
	~RNG() {}
};

#endif /* _DBUTL_RNG_H_ */

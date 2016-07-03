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

#ifndef _DBUTL_PTIMER_H_
#define _DBUTL_PTIMER_H_

#include <sys/resource.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace DBUtl {

class PTimer {

	private:
		struct timeval tim;
	#   ifdef __APPLE__
		struct rusage ru;
	#   else
		struct timespec ts;
	#   endif
		
		double real_start, real_end;
		double user_start, user_end;
		double system_start, system_end;
	
	public:
		PTimer() {}
		~PTimer() {}
		void start();
		void finish();
		double realResult();
		double userResult();
		double systemResult();
	
	};

#	ifdef DBUTL_INLINE
#		define INLINE inline
#		include "ptimer.hpp"
#	endif

} // namespace DBUtl

#endif /* _DBUTL_PTIMER_H_ */

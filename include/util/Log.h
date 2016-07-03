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
 *  Log.h
 *  DBUtl
 *
 *  Created by Alan Demers on 9/25/09.
 *  Copyright 2009 Cornell. All rights reserved.
 *
 *  Like <assert.h> this can be included multiple times, e.g.
 *
 *    #define DBUTL_NLOG
 *	  #undef  DBUTL_NDEBUG
 *	  #include "Log.h"
 *		...
 *	  #undef DBUTL_NLOG
 *	  #include "Log.h"
 *		...
 *
 * The DBUTL_NDEBUG and DBUTL_NLOG macros affect code generation
 *	at call sites.  If turned on, assertion checking / logging
 *	are *statically* disabled.  Dynamic enabling and disabling
 *	are independent of this.
 */

#ifndef _DBUTL_LOG_H_
#define _DBUTL_LOG_H_

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std::chrono;

extern clock_t startbuild;
extern high_resolution_clock::time_point var;

namespace DBUtl {
	
	/*
	 * Logging
	 */
	
	namespace Log {
		
		// severity levels
		
		enum Severity {
			ZERO		= 0,
			LOWEST		= 0,

			DEBUG		= 2,
			
			DEBUG0		= 2,
			DEBUG1		= 3,
			DEBUG2		= 4,
			DEBUG3		= 5,
			DEBUG4		= 6,
			
			TRACE		= 8,
			INFO		= 10,
			WARNING		= 12,
			ERR			= 14,
			CRIT		= 16,
			ALERT		= 18,
			EMERG		= 20,
			
			HIGHEST		= 127,
			DISABLED	= 127
		};
		
		struct MsgPrinter {
			Severity level;
			bool isAssertion;
			char * data;
			void print ( Severity sev, const char *fmt, ...);
			MsgPrinter * curry(Severity sev) { this->level = sev; return this; }
			void printCurried( const char *fmt, ... );
		};
		
		bool open( const char * fn, bool append = true );
		bool isOpen();
		void close();
		
		// dynamic disabling / severity filtering
		
		extern Severity _severityLevel;
		
		inline Severity getLevel() {
			return _severityLevel;
		}
		
		inline Severity setLevel(Severity sev) {
			Severity ans = _severityLevel;  _severityLevel = sev;  return ans;
		}
		
		inline bool isEnabled() { return _severityLevel < DISABLED; }
		
		// get a (one-shot) log message printer
		
		MsgPrinter * getLogger();
	}
	
	/*
	 * Assertions
	 */
	
	namespace Assert {
		
		extern bool _enabled;
		
		inline bool getEnabled() {
			return _enabled;
		}
		
		inline bool setEnabled(bool enable) {
			bool ans = _enabled;  _enabled = enable;  return ans;
		}
		
		Log::MsgPrinter * assertionFailed(const char * fn, int l, const char * e);
	}
	
}

#endif /* _DBUTL_LOG_H_ */


/*
 * Logging ...
 */

#undef DBUTL_LOG_ENABLED
#ifdef DBUTL_NLOG
#	define DBUTL_LOG_LEVEL	DBUtl::Log::DISABLED
#else
#	define DBUTL_LOG_LEVEL	DBUtl::Log::_severityLevel
#endif

#	undef DBUTL_LOG_FPRINTF
#	define DBUTL_LOG_FPRINTF(lev,args) do { \
		if( (lev) >= DBUTL_LOG_LEVEL ) { (DBUtl::Log::getLogger()->curry(lev))->printCurried args ; } \
	} while( 0 )

// DBUTLX and PRINT and PRINTF versions are DEPRECATED

#	undef DBUTL_LOG_PRINTF
#	define DBUTL_LOG_PRINTF if( DBUTL_LOG_LEVEL < DBUtl::Log::DISABLED ) \
		DBUtl::Log::getLogger()->print

#	undef DBUTL_LOG_PRINT
#	define DBUTL_LOG_PRINT DBUTL_LOG_PRINTF

#	undef DBUTLX_LOG_PRINTF
#	define DBUTLX_LOG_PRINTF DBUTL_LOG_PRINTF

#	undef DBUTLX_LOG_PRINT
#	define DBUTLX_LOG_PRINT DBUTL_LOG_PRINT


/*
 * Assertions ...
 */

#undef DBUTL_ASSERT_ENABLED
#ifdef DBUTL_NDEBUG
#	define DBUTL_ASSERT_ENABLED	false
#else
#	define DBUTL_ASSERT_ENABLED	DBUtl::Assert::_enabled
#endif

#	undef DBUTL_FASSERTF
#	define DBUTL_FASSERTF(e,args) do {\
		if( DBUTL_ASSERT_ENABLED && (!(e)) ) {\
			(DBUtl::Assert::assertionFailed(__FILE__, __LINE__, #e))->printCurried args ; \
		}\
	} while(0)

#	undef DBUTL_FASSERTF_ALWAYS
#	define DBUTL_FASSERTF_ALWAYS(e,args) do {\
		if( (!(e)) ) {\
			(DBUtl::Assert::assertionFailed(__FILE__, __LINE__, #e))->printCurried args ; \
		}\
	} while(0)

#	undef DBUTL_ASSERT
#	define DBUTL_ASSERT(e) DBUTL_FASSERTF(e, (""))

#	undef DBUTL_ASSERT_ALWAYS
#	define DBUTL_ASSERT_ALWAYS(e) DBUTL_FASSERTF_ALWAYS(e, (""))

// DBUTLX and and ASSERTF versions are all DEPRECATED:

#	undef DBUTL_ASSERTF
#	define DBUTL_ASSERTF(e) if( DBUTL_ASSERT_ENABLED && (!(e)) ) \
		(DBUtl::Assert::assertionFailed(__FILE__, __LINE__, #e))->printCurried

#	undef DBUTL_ASSERTF_ALWAYS
#	define DBUTL_ASSERTF_ALWAYS(e) if( !(e) ) \
		(DBUtl::Assert::assertionFailed(__FILE__, __LINE__, #e))->printCurried

#	undef DBUTLX_ASSERTF
#	define DBUTLX_ASSERTF DBUTL_ASSERTF

#	undef DBUTLX_ASSERTF_ALWAYS
#	define DBUTLX_ASSERTF_ALWAYS DBUTL_ASSERTF_ALWAYS

#	undef DBUTLX_ASSERT
#	define DBUTLX_ASSERT DBUTL_ASSERT

#	undef DBUTLX_ASSERT_ALWAYS
#	define DBUTLX_ASSERT_ALWAYS DBUTL_ASSERT_ALWAYS


/* no #endif here -- the #defines are unconditional */

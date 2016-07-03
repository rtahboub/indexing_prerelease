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
 *  Log.cpp
 *  DBUtl
 *
 *  Created by Alan Demers on 9/25/09.
 *  Copyright 2009 Cornell. All rights reserved.
 *
 */

#include "util/Log.h"
#include "util/pstdint.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <unistd.h> // for pid_t and getpid() TODO: Windows?
#include <stdexcept>

using namespace DBUtl;
using namespace DBUtl::Log;
using namespace DBUtl::Assert;


namespace DBUtl {	
	
	namespace Log {

		Severity _severityLevel = INFO;
		
		MsgPrinter * mpCache = 0;
		const int BUFSZ = 1024;
		
		FILE * logStream = NULL;
	}
	
	namespace Assert {
		
		bool _enabled = true;
		
	}
	
}

bool
DBUtl::Log::open( const char * fn, bool append ) {
	if( isOpen() ) close();
	FILE * stream = ( ((fn == 0) || (*fn == 0) || (strcmp(fn, "-") == 0))
					 ? stdout
					 : fopen( fn, (append ? "a" : "w") ) );
	logStream = stream;
	return logStream != NULL;
}

bool
DBUtl::Log::isOpen() {
	return (logStream != NULL);
}

void
DBUtl::Log::close() {
	if( (logStream != NULL) && (logStream != stdout) ) {
		fclose(logStream);
	}
	logStream = NULL;
}

void
DBUtl::Log::MsgPrinter::print(Severity sev, const char *fmt, ...) {
	this->level = sev;
	if( (level >= _severityLevel ) && (logStream != NULL) ) {
		fprintf(logStream, "%8ju: %s ", (intmax_t)getpid(), data); // TODO: Windows?
		// TODO: timestamp?
		va_list ap;
		va_start( ap, fmt );
		vfprintf(logStream, fmt, ap);
		va_end( ap );
		fflush(logStream);
	}
	if( isAssertion ) {
		if( logStream ) fflush(logStream);
		abort();
		throw std::logic_error("Assertion failure");
	}
	if( mpCache == 0 ) { mpCache = this; } else { free(this); }
}


void
DBUtl::Log::MsgPrinter::printCurried(const char *fmt, ...) {
	if( (this->level >= _severityLevel ) && (logStream != NULL) ) {
		fprintf(logStream, "%8ju: %s ", (intmax_t)getpid(), data); // TODO: Windows?
		// TODO: timestamp?
		va_list ap;
		va_start( ap, fmt );
		vfprintf(logStream, fmt, ap);
		va_end( ap );
		fflush(logStream);
	}
	if( isAssertion ) {
		if( logStream ) fflush(logStream);
		abort();
		throw std::logic_error("Assertion failure");
	}
	if( mpCache == 0 ) { mpCache = this; } else { free(this); }
}

namespace /* anon */ {
	
	DBUtl::Log::MsgPrinter * getMP(DBUtl::Log::Severity s, bool assn) {
		DBUtl::Log::MsgPrinter * mp = DBUtl::Log::mpCache;  DBUtl::Log::mpCache = 0;
		if( mp == 0 ) mp = (DBUtl::Log::MsgPrinter *)malloc( sizeof(*mp) + BUFSZ );
		mp->level = s;  mp->isAssertion = assn;
		mp->data = (char *)(mp+1);  mp->data[0] = 0;
		return mp;
	}
}

DBUtl::Log::MsgPrinter *
DBUtl::Log::getLogger() {
	return getMP(ZERO, false);
#if 0
	MsgPrinter * mp = mpCache;  mpCache = 0;
	if( mp == 0 ) mp = (MsgPrinter *)malloc( sizeof(MsgPrinter) + BUFSZ );
	mp->level = ZERO;  mp->isAssertion = true;  mp->data = (char *)(mp+1);  mp->data[0] = 0;
	return mp;
#endif
}



DBUtl::Log::MsgPrinter *
DBUtl::Assert::assertionFailed(const char * fn, int l, const char * e) {
	MsgPrinter * mp = getMP(DISABLED, true);
#if 0
	MsgPrinter * mp = mpCache;  mpCache = 0;
	if( mp == 0 ) mp = (MsgPrinter *)malloc( sizeof(MsgPrinter) + BUFSZ );
	mp->level = HIGHEST;  mp->isAssertion = true;  mp->data = (char *)(mp+1);
#endif
	snprintf(mp->data, BUFSZ,"ASSERTION FAILED (file %s line %d, \"%s\")",
			 fn, l, e);
	return mp;
}



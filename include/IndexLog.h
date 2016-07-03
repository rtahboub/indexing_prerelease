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


#ifndef _INDEX_LOG_H_
#define _INDEX_LOG_H_

#include <iostream>
#include <cstring>


namespace PSimIndex {

#ifndef LOG_LIMIT
#define LOG_LIMIT ZERO
#endif
#define LOG(level) \
  if (level < LOG_LIMIT) ; \
  else if (level < IndexLog::getLevel()) ; \
  else IndexLog::Get()

    /**
     * Logging levels.
     */
    enum IndexSeverity {
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
        ERR		= 14,
        CRIT		= 16,
        ALERT		= 18,
        EMERG		= 20,
			
        HIGHEST		= 127,
        DISABLED	= 127
    };


    /**
     * Simple C++ logging class. You can write a log message as follows:
     *    LOG(LEVEL) << this is a message << std::endl 
     * where level is of type IndexSeverity
     *
     * Logging can be disabled both statically (using the LOG_LIMIT macro) or
     * dynamically (using IndexLog::setLevel()). The log can be written to any
     * C++ ostream. By default it uses std::cout. 
     *
     * Note: This class is based on an article at
     *       http://www.ddj.com/cpp/201804215
     * Note: This class is NOT threadsafe. 
     * TODO: Add or combine this with the logging functionality in DBUtl.
     */
    class IndexLog {
    private:

        /**
         * The output stream to use for logging. Defaults to cout. 
         */
        static std::ostream* os;

        /** 
         * The current logging level. Only messages written at this level or
         * higher will be logged.
         */
        static IndexSeverity level;

    public:

        /** 
         * Converts the sring representation of a log level to the appropriate
         * IndexSeverity object.
         */
        static IndexSeverity strToLevel(const char* str) {
            if(strcmp(str, "ZERO") == 0) {
                return ZERO;
            }
            else if(strcmp(str, "LOWEST") == 0) {
                return LOWEST;
            }
            else if(strcmp(str, "DEBUG") == 0) {
                return DEBUG;
            }
            else if(strcmp(str, "DEBUG0") == 0) {
                return DEBUG0;
            }
            else if(strcmp(str, "DEBUG1") == 0) {
                return DEBUG1;
            }
            else if(strcmp(str, "DEBUG2") == 0) {
                return DEBUG2;
            }
            else if(strcmp(str, "DEBUG3") == 0) {
                return DEBUG3;
            }
            else if(strcmp(str, "DEBUG4") == 0) {
                return DEBUG4;
            }
            else if(strcmp(str, "TRACE") == 0) {
                return TRACE;
            }
            else if(strcmp(str, "INFO") == 0) {
                return INFO;
            }
            else if(strcmp(str, "WARNING") == 0) {
                return WARNING;
            }
            else if(strcmp(str, "ERR") == 0) {
                return ERR;
            }
            else if(strcmp(str, "CRIT") == 0) {
                return CRIT;
            }
            else if(strcmp(str, "ALERT") == 0) {
                return ALERT;
            }
            else if(strcmp(str, "EMERG") == 0) {
                return EMERG;
            }
            else if(strcmp(str, "HIGHEST") == 0) {
                return HIGHEST;
            }
            else if(strcmp(str, "DISABLED") == 0) {
                return DISABLED;
            }
            else {
                return DISABLED;
            }
        }

        /**
         * Returns the current gobal logging level. 
         */
        static IndexSeverity getLevel() {
            return level;
        }
        
        /**
         * Sets the current global logging level.  All messages written at
         * level or higher will be logged to the ostream os. 
         */
        static void setLevel(IndexSeverity myLevel) { 
            level = myLevel; 
        }

        /**
         * Sets the logging stream.
         */
        static void setStream(std::ostream& myOS) {
            os = &myOS;
        }

        /**
         * Returns the current logging stream. This should only be called
         * through the LOG macro. 
         */
        static std::ostream& Get() {
            (*os) << " - ";
            return (*os);
        }
    };
}
#endif /* _INDEX_LOG_H_ */

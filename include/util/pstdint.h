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

#ifndef _DBUTL_P_STDINT_H_
#define _DBUTL_P_STDINT_H_

/*
 * A version of stdint.h that works on
 *   Mac OS X 10.6 and beyond (__LP64__ model)
 *   Ubuntu 32-bit (long is 32-bit) TODO: this is untested
 *
 * This is not truly portable: it really only allows for the
 *   size of long to be either 64 bytes (__LP64__) or 32 bytes.
 *
 * 2009-10-01 ajd
 */

#include <climits>

#if (defined(_STDINT_H_) || defined(_STDINT_H)) && (!defined(__STDC_LIMIT_MACROS) || !defined(__STDC_CONSTANT_MACROS))
#	error Must define __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS to get ISO C99 semantics for stdint.h
#endif
#include <stdint.h>

#ifndef DBUTL_NO_ENHANCED_STDINT

#if !defined(INT16_MAX) || !defined(UINT16_MAX) || !defined(INT32_MAX) || !defined(UINT32_MAX) || !defined(INT64_MAX) || !defined(UINT64_MAX)
#	error stdint.h is broken on this platform
#endif


#ifndef INT16_C
# define INT16_C(v) ((int16_t) (v))
#endif
#ifndef UINT16_C
# define UINT16_C(v) ((uint16_t) (v))
#endif
#if (SHRT_MAX == INT16_MAX)
# ifndef PRINTF_INT16_MODIFIER
#  define PRINTF_INT16_MODIFIER "h"
# endif
#else
# error short is not 16 bits on this platform - not supported
#endif

#ifndef INT32_C
# define INT32_C(v) ((int32_t)(v))
#endif
#ifndef UINT32_C
# define UINT32_C(v) ((uint32_t)(v))
#endif
#if (INT_MAX == INT32_MAX)
# ifndef PRINTF_INT32_MODIFIER
#  define PRINTF_INT32_MODIFIER ""
# endif
#else
# error int is not 32 bits on this platform - not supported
#endif

#if (LONG_MAX == INT64_MAX)
# ifndef INT64_C
#  define INT64_C(v) v ## L
# endif
# ifndef UINT64_C
#  define UINT64_C(v) v ## UL
# endif
# ifndef PRINTF_INT64_MODIFIER
#  define PRINTF_INT64_MODIFIER "l"
# endif
#elif (LONG_MAX == INT32_MAX)
# ifndef INT64_C
#  define INT64_C(v) v ## LL
# endif
# ifndef UINT64_C
#  define UINT64_C(v) v ## ULL
# endif
# ifndef PRINTF_INT64_MODIFIER
#  define PRINTF_INT64_MODIFIER "ll"
# endif
#else
# error int is not 32 or 64 bits on this platform - not supported
#endif

# ifndef PRINTF_INTMAX_MODIFIER
#  define PRINTF_INTMAX_MODIFIER PRINTF_INT64_MODIFIER
# endif


# ifndef PRINTF_INT8_DEC_WIDTH
#  define PRINTF_INT8_DEC_WIDTH "3"
# endif
# ifndef PRINTF_INT16_DEC_WIDTH
#  define PRINTF_INT16_DEC_WIDTH "5"
# endif
# ifndef PRINTF_INT32_DEC_WIDTH
#  define PRINTF_INT32_DEC_WIDTH "10"
# endif
# ifndef PRINTF_INT64_DEC_WIDTH
#  define PRINTF_INT64_DEC_WIDTH "20"
# endif
# ifndef PRINTF_INTMAX_HEX_WIDTH
#  define PRINTF_INTMAX_HEX_WIDTH PRINTF_INT64_HEX_WIDTH
# endif

# ifndef PRINTF_INT8_HEX_WIDTH
#  define PRINTF_INT8_HEX_WIDTH "2"
# endif
# ifndef PRINTF_INT16_HEX_WIDTH
#  define PRINTF_INT16_HEX_WIDTH "4"
# endif
# ifndef PRINTF_INT32_HEX_WIDTH
#  define PRINTF_INT32_HEX_WIDTH "8"
# endif
# ifndef PRINTF_INT64_HEX_WIDTH
#  define PRINTF_INT64_HEX_WIDTH "16"
# endif
# ifndef PRINTF_INTMAX_DEC_WIDTH
#  define PRINTF_INTMAX_DEC_WIDTH PRINTF_INT64_DEC_WIDTH
# endif

#endif /* DBUTL_NO_ENHANCED_STDINT */

#endif /* _DBUTL_P_STDINT_H_ */

/* Copyright Andrea Di Iorio 2021
 * This file is part of RedBlackTree_linux_userspace
 * RedBlackTree_linux_userspace is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RedBlackTree_linux_userspace is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with RedBlackTree_linux_userspace.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#ifndef MACROS
#define MACROS

#include <stdio.h>
#include <stdlib.h>

#define TRUE    1
#define FALSE   0
#define true    TRUE
#define false   FALSE
///aux macro-functions
#define	ABS(a)				        ((a) > 0   ? (a) : -(a))
#define	MIN(a,b)			        ((a) < (b) ? (a) : (b))
#define MAX(a,b)			        ((a) > (b) ? (a) : (b))
#define AVG(a,b)                    ((a)/2 + (b)/2 + ((a)%2+(b)%2)/2)
#define SWAP(a,b)                   (a)=(a)^(b);(b)=(b)^(a);(a)=(a)^(b)
//ceil(x/y) with integers
#define INT_DIV_CEIL(x,y)		    ( ( (x) - 1) / (y) + 1 )
//2D ROW MAJOR 0based C-indexing wrap compute
#define IDX2D(i,j,nCols)            ((j) + (i)*(nCols))

///distribuite reminder @rem in group givin an extra +1 to the first @rem
#define UNIF_REMINDER_DISTRI(i,div,rem) \
    ( (div) + ( (i) < (rem) ? 1 : 0 ) )
#define UNIF_REMINDER_DISTRI_STARTIDX(i,div,rem) \
    ( (i) * (div) + MIN( (i),(rem) ) )

#define STATIC_ARR_ELEMENTS_N(arr)  (sizeof( (arr) ) / (sizeof(*(arr))))  
////STRING UTILS
#define _STR(s) #s
#define STR(s) _STR(s)
///CONCATENATE
//Concatenate preprocessor tokens A and B WITHOUT   expanding macro definitions
#define _CAT(a,b)    a ## b
//Concatenate preprocessor tokens A and B           EXPANDING macro definitions
#define CAT(a,b)    _CAT(a,b)
////PRINTS
#define CHIGHLIGHT                  "\33[1m\33[92m"
#define CCC                         CHIGHLIGHT
#define CHIGHLIGHTERR               "\33[31m\33[1m\33[44m"
#define CCCERR                      CHIGHLIGHTERR
#define CEND                        "\33[0m"
#define hprintsf(str,...)           printf( CHIGHLIGHT str CEND,__VA_ARGS__ ) 
#define hprintf(str)                printf( CHIGHLIGHT str CEND) 
#define ERRPRINTS(str,...)          fprintf( stderr, CHIGHLIGHTERR str CEND,__VA_ARGS__ )
#define ERRPRINT(str)               fprintf( stderr, CHIGHLIGHTERR str CEND )

#include <assert.h> 

///aux types
typedef char bool;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
//smart decimal type custom precision def build macro _DECIMAL_TRGT_PREC 
#ifndef _DECIMAL_TRGT_PREC
//dflt floating point precision & formatting chars
	#define _DECIMAL_TRGT_PREC	double
	#define _DECIMAL_TRGT_PREC_PR 	"%lf"
#else 
    //TODO SELECT WITH RESPECT TO THE EXPORTED TARGET DECIMAL TYPE
	#define _DECIMAL_TRGT_PREC_PR 	"%f"
#endif
typedef _DECIMAL_TRGT_PREC	decimal;


///EXTRA INCLUDE    --- cuda 
///assertionn are disabled at compile time by defining the NDEBUG preprocessor macro before including assert.h	s
//#ifdef ASSERT 	#include <assert.h> #endif

////////////////////////// LINUX KERNEL IMPORTED //////////////////////////////
typedef unsigned int u32;
typedef unsigned int cycles_t;

#define offsetof(TYPE, MEMBER)	((size_t)&((TYPE *)0)->MEMBER)

/** 
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *	TODO LESS_DEPENDENCIES
	removed:
		BUILD_BUG_ON_MSG(!__same_type(*(ptr), ((type *)0)->member) &&	\
				 !__same_type(*(ptr), void),			\
				 "pointer type mismatch in container_of()");	\
	
 */
#define container_of(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	((type *)(__mptr - offsetof(type, member))); })

#define NOOP(x)     x
//#define NOOP      do {} while(0)
#undef unlikely
#define unlikely(x) NOOP(x)
/*
 * Yes, this permits 64-bit accesses on 32-bit architectures. These will
 * actually be atomic in some cases (namely Armv7 + LPAE), but for others we
 * rely on the access being split into 2x32-bit accesses for a 32-bit quantity
 * (e.g. a virtual address) and a strong prevailing wind.
 */
#define compiletime_assert_rwonce_type(t)					\
	compiletime_assert(__native_word(t) || sizeof(t) == sizeof(long long),	\
		"Unsupported access size for {READ,WRITE}_ONCE().")
///TODO OVERWRITTEN FOR PORTING
#undef compiletime_assert_rwonce_type 
#define compiletime_assert_rwonce_type(t)   NOOP(t)
/*
 * Use __READ_ONCE() instead of READ_ONCE() if you do not require any
 * atomicity. Note that this may result in tears!
 */
#ifndef __READ_ONCE
#define __READ_ONCE(x)	(*(const volatile __unqual_scalar_typeof(x) *)&(x))
#endif

#define READ_ONCE(x)							\
({									\
	compiletime_assert_rwonce_type(x);				\
	__READ_ONCE(x);							\
})

#define __WRITE_ONCE(x, val)						\
do {									\
	*(volatile typeof(x) *)&(x) = (val);				\
} while (0)

#define WRITE_ONCE(x, val)						\
do {									\
	compiletime_assert_rwonce_type(x);				\
	__WRITE_ONCE(x, val);						\
} while (0)


/*
 * Use READ_ONCE_NOCHECK() instead of READ_ONCE() if you need to load a
 * word from memory atomically but without telling KASAN/KCSAN. This is
 * usually used by unwinding code when walking the stack of a running process.
 */
#define READ_ONCE_NOCHECK(x)						\
({									\
	compiletime_assert(sizeof(x) == sizeof(unsigned long),		\
		"Unsupported access size for READ_ONCE_NOCHECK().");	\
	(typeof(x))__read_once_word_nocheck(&(x));			\
})

#include <x86intrin.h>	//rdtsc
static inline cycles_t get_cycles(void)
{
	
	/**#ifndef CONFIG_X86_TSC
	if (!boot_cpu_has(X86_FEATURE_TSC))
		return 0;
	#endif */ ///TODO LESS_DEPENDENCIES

	return __rdtsc();
}

/**#define WARN_ON_ONCE(condition)	({				\
	static bool __section(".data.once") __warned;		\
	int __ret_warn_once = !!(condition);			\
								\
	if (unlikely(__ret_warn_once && !__warned)) {		\
		__warned = true;				\
		WARN_ON(1);					\
	}							\
	unlikely(__ret_warn_once);				\
}) */ //TODO LESS_DEPENDENCIES
#include <assert.h>
#define WARN_ON_ONCE(condition)	assert( !(condition) )

#define div_u64(a,b)	( a / b )
#define kfree(x)		free(x)
#endif 	//MACROS

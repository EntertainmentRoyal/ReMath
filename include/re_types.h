#ifndef RE_TYPE_H
#define RE_TYPE_H

#include <stdint.h>
#include <stdbool.h>

typedef float  		RE_f32;
typedef double 		RE_f64;

typedef int32_t 	RE_i32;
typedef uint32_t	RE_i64;


#ifndef RE_INLINE
	#if defined(__STDC_VERSOIN__) && __STDC_VERSION__ >= 199901L
		#define RE_INLINE static inline
	#else 
		#define RE_INLINE static
	#endif
#endif

#endif /* RE_TYPE_H */

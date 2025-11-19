#ifndef RE_CONSTANTS_H
#define RE_CONSTANTS_H

#include "re_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// single precision constants (default)
#define RE_PI_F			        3.14159265358979323846f
#define RE_TAU_F		        6.28318530717958647692f
#define RE_INV_PI_F		        0.31830988618f		// (1.0f / RE_PI_F)
#define RE_DEG2RAD_F		    0.01745329251f		// (RE_PI_F / 180.0f)
#define RE_RAD2DEG_F		    57.2957795131f		// (180.0f / RE_PI_F)

#define RE_EPSILON_F		    1e-6f
#define RE_SMALL_EPSILON_F	    1e-12f

// double precision variants
#define RE_PI_D   		        3.141592653589793238462643383279502884
#define RE_TAU_D  		        6.283185307179586476925286766559005768

#ifdef __cplusplus
}
#endif

#endif /* RE_CONSTANTS_H */

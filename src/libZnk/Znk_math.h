#ifndef INCLUDE_GUARD__Znk_math_h__
#define INCLUDE_GUARD__Znk_math_h__

#include <Znk_base.h>
#include <math.h>

/***
 * VC等、これが定義されていないmath.hがある.
 */
#ifndef M_PI
#  define M_PI		3.14159265358979323846
#endif

/***
 * ガンマ関数
 */
double
ZnkMath_gamma( double z );

#endif /* INCLUDE_GUARD */

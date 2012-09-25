/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include "int.h"

inline fixed float_to_fixed (float a)
{
	return (fixed)(a * (1 << FRAC));
}

inline fixed fixmul (fixed a, fixed b)
{
	return (fixed)((int32) a * b >> FRAC);
}

inline fixed fixdiv (fixed a, fixed b)
{
	return (fixed)(((int32) a << FRAC) / b);
}

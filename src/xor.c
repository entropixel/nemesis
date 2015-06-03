/* See LICENSE file for copyright and licensing details */
#include "int.h"
#include "xor.h"

static xor_t internal;

void xsrand (uint32 seed)
{
	xsrand_r (&internal, seed);
	return;
}

void xsrand_r (xor_t *x, uint32 seed)
{
	if (!seed)
		x->rs [0] = seed - 1;
	else
		x->rs [0] = seed;

	x->seed = x->rs [0];

	x->rs [1] = x->rs [0] << 1;
	x->rs [2] = x->rs [1] << 1;
	x->rs [3] = x->rs [2] << 1;

	return;
}

uint32 xrand (void)
{
	return xrand_r (&internal);
}

uint32 xrand_r (xor_t *x)
{
	register uint32 tmp;

	tmp = x->rs [0] ^ (x->rs [0] << 4);

	x->rs [0] = x->rs [1];
	x->rs [1] = x->rs [2];
	x->rs [2] = x->rs [3];

	return x->rs [3] = (x->rs [3] ^ (x->rs [3] >> 13)) ^ (tmp ^ (tmp >> 13));
}

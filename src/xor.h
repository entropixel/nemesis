/* See LICENSE file for copyright and licensing details */
#ifndef XOR_H__
#define XOR_H__

typedef struct
{
	uint32 seed;
	uint32 rs [4];
} xor_t;

void xsrand (uint32 seed);
void xsrand_r (xor_t *x, uint32 seed);
uint32 xrand (void);
uint32 xrand_r (xor_t *x);

#endif // XOR_H__

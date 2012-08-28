/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef LEVEL_H__
#define LEVEL_H__

struct tile_t;

typedef struct
{
	uint8 w;
	uint8 h;

	struct tile_t *tiles;
	uint32 *offs;
} level_t;

level_t *level_new (uint8 w, uint8 h);
void level_edit_tile (int8 offs);

#endif // LEVEL_H__

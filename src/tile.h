/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef TILE_H__
#define TILE_H__

typedef struct
{
	unsigned short x;
	unsigned short y;
} tileoffs_t;

typedef struct
{
	SDL_Texture *sheet;
	tileoffs_t offs;
	unsigned short flags; // todo
} tile_t;

extern tileoffs_t dungeon_tileoffs [];

#endif // TILE_H__

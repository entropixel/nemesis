/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <SDL.h>

#include "tile.h"

tileoffs_t dungeon_tileoffs [] =
{
	{ 0, 0 }, // floor
	{ 2, 0 }, // north wall
	{ 2, 1 },
	{ 2, 2 },
	{ 3, 0 }, // east wall
	{ 3, 1 },
	{ 3, 2 },
	{ 4, 0 }, // south wall
	{ 4, 1 },
	{ 4, 2 },
	{ 5, 0 }, // west wall
	{ 5, 1 },
	{ 5, 2 },
	{ 6, 0 }, // inner northeast corner
	{ 6, 1 },
	{ 6, 2 },
	{ 7, 0 }, // inner northwest corner
	{ 7, 1 },
	{ 7, 2 },
	{ 8, 0 }, // inner southeast corner
	{ 8, 1 },
	{ 8, 2 },
	{ 9, 0 }, // inner southwest corner
	{ 9, 1 },
	{ 9, 2 },
};

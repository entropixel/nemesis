/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <SDL.h>

#include "int.h"
#include "tile.h"

tileoffs_t dungeon_tileoffs [] =
{
	{ 0, 0 }, // floor
	{ 0, 1 },
	{ 2, 0 }, // north wall
	{ 2, 1 },
	{ 2, 2 },
	{ 2, 3 },
	{ 2, 4 },
	{ 2, 5 },
	{ 3, 0 }, // east wall
	{ 3, 1 },
	{ 3, 2 },
	{ 3, 3 },
	{ 3, 4 },
	{ 3, 5 },
	{ 4, 0 }, // south wall
	{ 4, 1 },
	{ 4, 2 },
	{ 4, 3 },
	{ 4, 4 },
	{ 4, 5 },
	{ 5, 0 }, // west wall
	{ 5, 1 },
	{ 5, 2 },
	{ 5, 3 },
	{ 5, 4 },
	{ 5, 5 },
	{ 6, 0 }, // inner northeast corner
	{ 6, 1 },
	{ 6, 2 },
	{ 6, 3 },
	{ 6, 4 },
	{ 6, 5 },
	{ 7, 0 }, // inner northwest corner
	{ 7, 1 },
	{ 7, 2 },
	{ 7, 3 },
	{ 7, 4 },
	{ 7, 5 },
	{ 8, 0 }, // inner southeast corner
	{ 8, 1 },
	{ 8, 2 },
	{ 8, 3 },
	{ 8, 4 },
	{ 8, 5 },
	{ 9, 0 }, // inner southwest corner
	{ 9, 1 },
	{ 9, 2 },
	{ 9, 3 },
	{ 9, 4 },
	{ 9, 5 },
	{ 10, 0 }, // outer northeast corner
	{ 10, 1 },
	{ 10, 2 },
	{ 10, 3 },
	{ 10, 4 },
	{ 10, 5 },
	{ 11, 0 }, // outer northwest corner
	{ 11, 1 },
	{ 11, 2 },
	{ 11, 3 },
	{ 11, 4 },
	{ 11, 5 },
	{ 12, 0 }, // outer southeast corner
	{ 12, 1 },
	{ 12, 2 },
	{ 12, 3 },
	{ 12, 4 },
	{ 12, 5 },
	{ 13, 0 }, // outer southwest corner
	{ 13, 1 },
	{ 13, 2 },
	{ 13, 3 },
	{ 13, 4 },
	{ 13, 5 },
	{ 2, 8 }, // north stairs
	{ 2, 9 },
	{ 2, 10 },
	{ 2, 11 },
	{ 3, 8 }, // east stairs
	{ 3, 9 },
	{ 3, 10 },
	{ 3, 11 },
	{ 4, 8 }, // south stairs
	{ 4, 9 },
	{ 4, 10 },
	{ 4, 11 },
	{ 5, 8 }, // west stairs
	{ 5, 9 },
	{ 5, 10 },
	{ 5, 11 },
	{ 15, 15 }, // nothingness
};

int32 dungeon_tileoffs_size = sizeof (dungeon_tileoffs) / sizeof (*dungeon_tileoffs);

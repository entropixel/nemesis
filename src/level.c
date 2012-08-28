/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL.h>

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "level.h"
#include "tile.h"

level_t *level_new (uint8 w, uint8 h)
{
	level_t *ret = malloc (sizeof (level_t));

	if (!ret)
		return NULL;

	ret->w = w;
	ret->h = h;

	ret->tiles = malloc (w * h * sizeof (struct tile_t));
	ret->offs = malloc (w * h * sizeof (int32));

	if (!ret->tiles || !ret->offs)
	{
		free (ret->tiles);
		free (ret->offs);
		free (ret);
		return NULL;
	}

	memset (ret->tiles, 0, sizeof (struct tile_t) * w * h);
	memset (ret->offs, 0, sizeof (int32) * w * h);

	return ret;
}

extern level_t *level; // FIXME loool
extern obj_t *player;
void update_tiles (void);
void level_edit_tile (int8 offs)
{
	static uint32 lastused = 0;
	uint32 *level_offs = &(level->offs [((player->hitbox.x + player->hitbox.w / 2) / 16) * level->w + ((player->hitbox.y + player->hitbox.h / 2) / 16)]);

	if (!(*level_offs))
		*level_offs = dungeon_tileoffs_size;

	if (offs)
	{
		*level_offs += offs;
		*level_offs %= dungeon_tileoffs_size;
		lastused = *level_offs;
	}
	else
	{
		*level_offs = lastused;
	}

	update_tiles ();

	return;
}

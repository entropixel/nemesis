/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <SDL.h>

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "tile.h"
#include "player.h"
#include "input.h"

extern tile_t levtiles [16] [10];

void player_thinker (obj_t *obj)
{
	if (keymask)
	{
		if (obj->frame == char_anim_stand)
			obj_set_frame (obj, char_anim_walk1);

		switch (keymask)
		{
			case UPK:
				obj_set_rot (obj, ROT_UP);
				obj->y -= 1.41f;
			break;
			case UPK | RIGHTK:
				obj_set_rot (obj, ROT_UPRIGHT);
				obj->x += 1.0f;
				obj->y -= 1.0f;
			break;
			case RIGHTK:
				obj_set_rot (obj, ROT_RIGHT);
				obj->x += 1.41f;
			break;
			case RIGHTK | DOWNK:
				obj_set_rot (obj, ROT_DOWNRIGHT);
				obj->x += 1.0f;
				obj->y += 1.0f;
			break;
			case DOWNK:
				obj_set_rot (obj, ROT_DOWN);
				obj->y += 1.41f;
			break;
			case DOWNK | LEFTK:
				obj_set_rot (obj, ROT_DOWNLEFT);
				obj->x -= 1.0f;
				obj->y += 1.0f;
			break;
			case LEFTK:
				obj_set_rot (obj, ROT_LEFT);
				obj->x -= 1.41f;
			break;
			case LEFTK | UPK:
				obj_set_rot (obj, ROT_UPLEFT);
				obj->x -= 1.0f;
				obj->y -= 1.0f;
			break;
			default:
				obj_set_frame (obj, char_anim_stand);
			break;
		}

		obj_collide_tiles (obj, *levtiles, sizeof (*levtiles) / sizeof (**levtiles));

	}
	else
		obj_set_frame (obj, char_anim_stand);

	return;
}

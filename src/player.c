/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <SDL.h>

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "level.h"
#include "tile.h"
#include "player.h"
#include "input.h"

extern level_t *level;

void player_thinker (obj_t *obj)
{
	if (keymask & (UPK | RIGHTK | DOWNK | LEFTK) && !(keymask & ATK) && obj->frame < char_anim_punch1)
	{
		if (obj->frame == char_anim_idle1 || obj->frame == char_anim_idle2)
			obj_set_frame (obj, char_anim_walk1);

		switch (keymask)
		{
			case UPK:
				obj_set_rot (obj, ROT_UP);
				obj->deltay = -23;
			break;
			case UPK | RIGHTK:
				obj_set_rot (obj, ROT_UPRIGHT);
				obj->deltax = 16;
				obj->deltay = -16;
			break;
			case RIGHTK:
				obj_set_rot (obj, ROT_RIGHT);
				obj->deltax = 23;
			break;
			case RIGHTK | DOWNK:
				obj_set_rot (obj, ROT_DOWNRIGHT);
				obj->deltax = 16;
				obj->deltay = 16;
			break;
			case DOWNK:
				obj_set_rot (obj, ROT_DOWN);
				obj->deltay = 23;
			break;
			case DOWNK | LEFTK:
				obj_set_rot (obj, ROT_DOWNLEFT);
				obj->deltax = -16;
				obj->deltay = 16;
			break;
			case LEFTK:
				obj_set_rot (obj, ROT_LEFT);
				obj->deltax = -23;
			break;
			case LEFTK | UPK:
				obj_set_rot (obj, ROT_UPLEFT);
				obj->deltax = -16;
				obj->deltay = -16;
			break;
			default:
				obj_set_frame (obj, char_anim_idle1);
			break;
		}

		obj->x += obj->deltax;
		obj->y += obj->deltay;
		obj_collide_tiles (obj, level->tiles, level->w);
		obj->deltax = obj->deltay = 0;

	}
	else if (keymask & ATK && obj->frame < char_anim_punch1)
	{
		obj_set_frame (obj, char_anim_punch1);
	}
	else if (obj->frame != char_anim_idle1 && obj->frame != char_anim_idle2 && obj->frame < char_anim_punch1)
		obj_set_frame (obj, char_anim_idle1);

	return;
}

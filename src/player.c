/* See LICENSE file for copyright and licensing details */
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
	obj_t *it = obj_list_head;

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

		obj_collide_tiles (obj, level->tiles, level->w);

		while (it)
		{
			if (it->hitbox.w && it->hitbox.h && it != obj)
				obj_collide_hitbox (obj, &(it->hitbox));

			it = it->next;
		}

		obj->hitbox.x += obj->deltax;
		obj->hitbox.y += obj->deltay;
		obj->x = obj->hitbox.x - obj->hitbox.offsx;
		obj->y = obj->hitbox.y - obj->hitbox.offsy;
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

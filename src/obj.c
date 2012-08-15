/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "tile.h"

obj_t *obj_list_head = NULL, *obj_list_tail = NULL;

obj_t *obj_create (float x, float y, SDL_Texture *tex, anim_t *anim, uint16 frame, uint8 rot)
{
	obj_t *ret = malloc (sizeof (obj_t));

	if (!ret)
		return NULL;

	memset (ret, 0, sizeof (obj_t));

	ret->x = x;
	ret->y = y;
	ret->tex = tex;
	ret->anim = anim;
	ret->show.w = ret->dest.w = ret->anim->w;
	ret->show.h = ret->dest.h = ret->anim->h;
	ret->dest.x = (int16) x;
	ret->dest.y = (int16) y;
	ret->next = NULL;
	obj_set_frame (ret, frame);
	obj_set_rot (ret, rot);

	if (!obj_list_head)
	{
		obj_list_head = obj_list_tail = ret;
	}
	else
	{
		obj_list_tail->next = ret;
		obj_list_tail = ret;
	}

	return ret;
}

void obj_set_hitbox (obj_t *obj, uint8 x, uint8 y, uint16 w, uint16 h)
{
	obj->hitbox.x = ((int16)obj->x) + x;
	obj->hitbox.y = ((int16)obj->y) + y;
	obj->hitbox.w = w;
	obj->hitbox.h = h;

	obj->hitb_x = x;
	obj->hitb_y = y;

	return;
}

void obj_set_frame (obj_t *obj, uint16 frame)
{
	obj->frame = frame;
	obj->frametics = obj->anim->frames [obj->frame].duration;
	obj->show.y = obj->anim->frames [obj->frame].pos * obj->anim->h;
	return;
}

void obj_set_rot (obj_t *obj, uint8 rot)
{
	obj->rot = rot;
	obj->show.x = obj->anim->rots [obj->rot] * obj->anim->w;
	return;
}

void obj_adv_frame (obj_t *obj)
{
	if (obj->frametics > 0)
		obj->frametics --;

	if (!obj->frametics)
		obj_set_frame (obj, obj->anim->frames [obj->frame].next);

	return;
}

void obj_collide_tiles (obj_t *obj, struct tile_t *tiles, uint16 width)
{
	obj->hitbox.x = ((int16) obj->x) + obj->hitb_x;
	obj->hitbox.y = ((int16) obj->y) + obj->hitb_y;

	if (tiles [((obj->hitbox.x + obj->hitbox.w / 2) / 16) * width + obj->hitbox.y / 16].flags & TF_SOLID)
		obj->y -= (obj->hitbox.y % 16) - 16;
	if (tiles [((obj->hitbox.x + obj->hitbox.w / 2) / 16) * width + (obj->hitbox.y + obj->hitbox.h) / 16].flags & TF_SOLID)
		obj->y -= (obj->hitbox.y % 16) + obj->hitbox.h - 16;
	if (tiles [(obj->hitbox.x / 16) * width + (obj->hitbox.y + obj->hitbox.h / 2) / 16].flags & TF_SOLID)
		obj->x -= (obj->hitbox.x % 16) - 16;
	if (tiles [((obj->hitbox.x + obj->hitbox.w) / 16) * width + (obj->hitbox.y + obj->hitbox.h / 2) / 16].flags & TF_SOLID)
		obj->x -= (obj->hitbox.x % 16) + obj->hitbox.w - 16;

	obj->hitbox.x = ((int16) obj->x) + obj->hitb_x;
	obj->hitbox.y = ((int16) obj->y) + obj->hitb_y;

	return;
}

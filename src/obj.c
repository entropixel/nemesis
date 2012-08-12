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

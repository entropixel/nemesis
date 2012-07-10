/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

#include "obj.h"
#include "anim.h"

obj_t *obj_create (float x, float y, SDL_Texture *tex, anim_t *anim, unsigned short frame, unsigned char rot)
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
	ret->dest.x = (short) x;
	ret->dest.y = (short) y;
	obj_set_frame (ret, frame);
	obj_set_rot (ret, rot);

	return ret;
}

void obj_set_frame (obj_t *obj, unsigned short frame)
{
	obj->frame = frame;
	obj->frametics = obj->anim->frames [obj->frame].duration;
	obj->show.y = obj->anim->frames [obj->frame].pos * obj->anim->h;
	return;
}

void obj_set_rot (obj_t *obj, unsigned char rot)
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

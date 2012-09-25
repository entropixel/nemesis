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

obj_t *obj_create (fixed x, fixed y, SDL_Texture *tex, anim_t *anim, uint16 frame, uint8 rot, obj_f thinker, obj_f deinit)
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
	ret->dest.x = x >> FRAC;
	ret->dest.y = y >> FRAC;
	ret->thinker = thinker;
	ret->deinit = deinit;
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

void obj_destroy (obj_t *obj)
{
	obj_t *it = obj_list_head;

	if (!obj)
		return;

	// remove object from linked list
	if (it == obj)
		obj_list_head = it->next;
	else
	{
		while (it->next != obj)
			it = it->next;

		it->next = it->next->next;
		if (!it->next)
			obj_list_tail = it;
	}

	if (obj->deinit) // call deconstructor
		obj->deinit (obj);

	free (obj);
	return;
}

void obj_set_hitbox (obj_t *obj, fixed x, fixed y, fixed w, fixed h)
{
	obj->hitbox.x = obj->x + x;
	obj->hitbox.y = obj->y + y;
	obj->hitbox.w = w;
	obj->hitbox.h = h;

	obj->hitbox.offsx = x;
	obj->hitbox.offsy = y;

	return;
}

// give center x/y of object
inline fixed obj_centerx (obj_t *obj)
{
	return obj->hitbox.x + obj->hitbox.w / 2;
}

inline fixed obj_centery (obj_t *obj)
{
	return obj->hitbox.y + obj->hitbox.h / 2;
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

void obj_point (obj_t *obj)
{
	static fixed mindel = 2; // minimum delta

	if (obj->deltax < -mindel) // looking left
	{
		if (obj->deltay < -mindel) // up-left
			obj_set_rot (obj, ROT_UPLEFT);
		else if (obj->deltay > mindel) // down-left
			obj_set_rot (obj, ROT_DOWNLEFT);
		else
			obj_set_rot (obj, ROT_LEFT);
	}
	else if (obj->deltax > mindel)
	{
		if (obj->deltay < -mindel) // up-right
			obj_set_rot (obj, ROT_UPRIGHT);
		else if (obj->deltay > mindel) // down-right
			obj_set_rot (obj, ROT_DOWNRIGHT);
		else
			obj_set_rot (obj, ROT_RIGHT);
	}
	else if (obj->deltay < -mindel)
		obj_set_rot (obj, ROT_UP);
	else if (obj->deltay > mindel)
		obj_set_rot (obj, ROT_DOWN);

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

void obj_do_advframes (void) // wrap the above
{
	obj_t *it = obj_list_head;

	while (it)
	{
		obj_adv_frame (it);
		it = it->next;
	}

	return;
}

#define SHB_X 1
#define SHB_Y 2

static inline void sethitbox (obj_t *obj, uint32 flags)
{
	if (flags & SHB_X)
		obj->hitbox.x = obj->x + obj->hitbox.offsx;
	if (flags & SHB_Y)
		obj->hitbox.y = obj->y + obj->hitbox.offsy;
}

// Set bounding box and hit values
static inline void obj_set_bb (obj_t *obj, int32 *boxcoord, uint8 *hit, struct tile_t *tiles, uint16 width)
{
	int32 i;
	/* Determine box coords */
	// X-Axis
	boxcoord [0] = boxcoord [4] = obj->hitbox.x / 16;
	boxcoord [2] = boxcoord [6] = (obj->hitbox.x + obj->hitbox.w - 1) / 16;
	
	// Y Axis
	boxcoord [1] = boxcoord [3] = obj->hitbox.y / 16;
	boxcoord [5] = boxcoord [7] = (obj->hitbox.y + obj->hitbox.h - 1) / 16;
	
	/* Determine corner collision */
	for (i = 0; i < 4; i++)
		hit [i] = tiles [(boxcoord [i * 2] * width) + boxcoord [i * 2 + 1]].flags & TF_SOLID;

	return;
}

void obj_collide_tiles (obj_t *obj, struct tile_t *tiles, uint16 width)
{
	// (0)----(1)
	//  |      |  [x, y]
	//  |      | boxcoord = you
	// (2)----(3)
	int32 boxcoord [4] [2];
	uint8 hit [4];
	int16 oldx, oldy;

	/* Ignore no-clips */
	if (42 || obj->flags & OF_NOCLIP)
	{
		sethitbox (obj, SHB_X | SHB_Y);
		return;
	}

	sethitbox (obj, SHB_X);
	obj_set_bb (obj, *boxcoord, hit, tiles, width);
	oldx = obj->hitbox.x;

	/* X Axis Correction */
	// Moving Left, Left collides
	if (obj->deltax < 0 && (hit [0] || hit [2]))
	{
		obj->hitbox.x -= (obj->hitbox.x % 16) - 16;
		obj->deltax = 0;
	}

	// Moving Right, right collides
	if (obj->deltax > 0 && (hit [1] || hit [3]))
	{
		obj->hitbox.x -= (obj->hitbox.x % 16) + obj->hitbox.w - 16;
		obj->deltax = 0;
	}

	sethitbox (obj, SHB_Y);
	obj_set_bb (obj, *boxcoord, hit, tiles, width);
	oldy = obj->hitbox.y;

	/* Y Axis Correction */
	// Moving Up, top collides
	if (obj->deltay < 0 && (hit [0] || hit [1]))
	{
		obj->hitbox.y -= (obj->hitbox.y % 16) - 16;
		obj->deltay = 0;
	}

	// Moving Down, bottom collides
	if (obj->deltay > 0 && (hit [2] || hit [3]))
	{
		obj->hitbox.y -= (obj->hitbox.y % 16) + obj->hitbox.h - 16;
		obj->deltay = 0;
	}
	
	// Correct position
	obj->x += obj->hitbox.x - oldx;
	obj->y += obj->hitbox.y - oldy;
	return;
}

void obj_do_thinkers (void)
{
	obj_t *it = obj_list_head;

	while (it)
	{
		if (it->thinker)
			it->thinker (it);

		it = it->next;
	}

	return;
}

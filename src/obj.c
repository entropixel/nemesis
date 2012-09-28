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

uint8 obj_collide_hitbox (obj_t *obj, hitbox_t *testbox)
{
	static fixed fudge = 24;
	int32 a;
	uint8 ret = 0;
	fixed *delta;
	fixed *omina, omaxa, tmina, tmaxa;
	fixed *ominb, omaxb, tminb, tmaxb;

	if (obj->flags & OF_NOCLIP)
		return 0;

	for (a = 0; a < 2; a++)
	{
		// 'a' is current axis, 'b' is opposite
		if (!a) // Y Axis
		{
			if (obj->hitbox.y + obj->deltay > testbox->y + testbox->h
			||  obj->hitbox.y + obj->hitbox.h + obj->deltay < testbox->y) // not touching
				continue;

			tmina = testbox->y;
			tmaxa = tmina + testbox->h;
			omina = &obj->hitbox.y;
			omaxa = *omina + obj->hitbox.h - 1;
			tminb = testbox->x;
			tmaxb = tminb + testbox->w;
			ominb = &obj->hitbox.x;
			omaxb = *ominb + obj->hitbox.w - 1;
			delta = &obj->deltay;
		}
		else // X axis
		{
			if (obj->hitbox.x + obj->deltax > testbox->x + testbox->w
			||  obj->hitbox.x + obj->hitbox.w + obj->deltax < testbox->x) // not touching
				continue;

			tmina = testbox->x;
			tmaxa = tmina + testbox->w;
			omina = &obj->hitbox.x;
			omaxa = *omina + obj->hitbox.w - 1;
			tminb = testbox->y;
			tmaxb = tminb + testbox->h;
			ominb = &obj->hitbox.y;
			omaxb = *ominb + obj->hitbox.h - 1;
			delta = &obj->deltax;
		}

		// test for actual intersection
		if ((*ominb > tminb && *ominb < tmaxb) || (omaxb > tminb && omaxb < tmaxb) || (tminb > *ominb && tmaxb < omaxb))
		{
			fixed cost;

			// fudge this a bit
			if (abs (*ominb - tmaxb) < fudge || abs (omaxb - tminb) < fudge)
				continue;

			// Which side are we going into?
			if (*delta < 0 && *omina + *delta <= tmaxa) // left/up
			{
				cost = abs (tmaxa - *omina);

				// Only correct if this is the cheapest way out
				if (cost <= abs (omaxb - tminb) && cost <= abs (*ominb - tmaxb))
				{
					*delta = 0;
					*omina = tmaxa;
				}
			}
			else if (*delta > 0 && omaxa + *delta >= tmina) // right/down
			{
				cost = abs (tmina - omaxa);

				if (cost <= abs (omaxb - tminb) && cost <= abs (*ominb - tmaxb))
				{
					*delta = 0;
					*omina = tmina - (1 + omaxa - *omina);
				}
			}
			ret = 1;
		}
	}

	return ret;
}

uint8 obj_collide_tiles (obj_t *obj, struct tile_t *tiles, uint8 w)
{
	uint8 i, j;
	uint32 ret;

	for (i = (obj_centerx (obj) >> FRAC * 2) - 1; i <= (obj_centerx (obj) >> FRAC * 2) + 1; i++)
		for (j = (obj_centery (obj) >> FRAC * 2) - 1; j <= (obj_centery (obj) >> FRAC * 2) + 1; j++)
			if (tiles [i * w + j].hitbox.w && tiles [i * w + j].hitbox.h)
				ret += obj_collide_hitbox (obj, (hitbox_t*)&(tiles [i * w + j].hitbox));

	return !!ret;
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

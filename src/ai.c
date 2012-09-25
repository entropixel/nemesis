/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "int.h"
#include "xor.h"
#include "anim.h"
#include "obj.h"
#include "level.h"
#include "tile.h"
#include "ai.h"

#define MD(a,b) ((abs (a->x - b->x) + abs (a->y - b->y)) * 10)

extern SDL_Renderer *rndr;

uint8 iopen = 0, iclose = 0;
mapnode_t *ai_get_path (obj_t *obj, obj_t *targ, level_t *l)
{
	mapnode_t *ret = NULL; // the path we return
	mapnode_t nodelist [l->w * l->h];
	mapnode_t *opened [l->w * l->h], *closed [l->w * l->h];
	mapnode_t *goal;

	iopen = iclose = 0;

	int32 i, j;

	// populate nodelist, init open/closed list
	for (i = 0; i < l->w; i++)
		for (j = 0; j < l->h; j++)
		{
			opened [i * l->w + j] = closed [i * l->w + j] = NULL;
			if (l->tiles [i * l->w + j].flags & TF_SOLID)
				nodelist [i * l->w + j].type = N_BLOCKED;
			else
				nodelist [i * l->w + j].type = N_UNSET;

			nodelist [i * l->w + j].x = i;
			nodelist [i * l->w + j].y = j;

			nodelist [i * l->w + j].g = nodelist [i * l->w + j].h = 0;
			nodelist [i * l->w + j].parent = NULL;
		}

	opened [iopen++] = &(nodelist [(obj_centerx (obj) >> FRAC * 2) * l->w + (obj_centery (obj) >> FRAC * 2)]);
	opened [iopen - 1]->type = N_OPENED;

	goal = &nodelist [(obj_centerx (targ) >> FRAC * 2) * l->w + (obj_centery (targ) >> FRAC * 2)];

	while (1)
	{
		mapnode_t *current = opened [0];
		j = 0;
		for (i = 0; i < iopen; i++)
		{
			// set scores
			if (!opened [i]->h && opened [i] != goal)
				opened [i]->h = MD (opened [i], goal);

			if (opened [i]->parent && !opened [i]->g)
			{
				opened [i]->g = opened [i]->parent->g;
				if (opened [i]->x != opened [i]->parent->x && opened [i]->y != opened [i]->parent->y)
					opened [i]->g += 14;
				else
					opened [i]->g += 10;
			}

			if (!current || opened [i]->g + opened [i]->h < current->g + current->h)
			{
				current = opened [i];
				j = i; // back up the index to the best one
			}
		}

		if (current == goal)
		{
			mapnode_t *it = current;
			uint16 numnodes = 0;

			while (it)
			{
				numnodes ++;
				it = it->parent;
			}

			ret = malloc (numnodes * sizeof (mapnode_t));

			if (!ret)
				break;

			while (current)
			{
				memcpy (&(ret [--numnodes]), current, sizeof (mapnode_t));
				current = current->parent;
			}

			return ret;
		}

		// move node to closed list
		closed [iclose++] = opened [j];
		while (opened [j])
		{
			opened [j] = opened [j + 1];
			j ++;
		}

		if (iopen)
			iopen --;
		else
			break;

		current->type = N_CLOSED;

		for (i = current->x - 1; i <= current->x + 1; i++)
			for (j = current->y - 1; j <= current->y + 1; j++)
			{
				uint32 tmpg = current->g;

				if (i < 0 || i > l->w - 1 || j < 0 || j > l->h - 1)
					continue;

				if (nodelist [i * l->w + current->y].type == N_BLOCKED || nodelist [current->x * l->w + j].type == N_BLOCKED)
					continue;

				if (i != current->x && j != current->y)
					tmpg += 14;
				else
					tmpg += 10;

				if (nodelist [i * l->w + j].type == N_UNSET) // neighbor isn't in either set, add to open set
				{
					opened [iopen++] = &nodelist [i * l->w + j];
					opened [iopen - 1]->g = tmpg;

					opened [iopen - 1]->type = N_OPENED;
					opened [iopen - 1]->parent = current;
				}
				else if (nodelist [i * l->w + j].type == N_OPENED && tmpg < nodelist [i * l->w + j].g)
				{
					nodelist [i * l->w + j].g = tmpg;
					nodelist [i * l->w + j].parent = current;
				}
			}
	}

	return NULL;
}

extern level_t *level;
extern uint8 editmode;
#define max(a,b) ((a > b) ? a : b)
void ai_thinker (obj_t *obj)
{
	aidata_t *data = obj->data;
	fixed distx, disty;

	if (editmode)
		return;

	if (data->state == ai_chase)
	{
		// init/update node list if we need to
		if (!data->nodelist || obj_centerx (data->target) >> FRAC * 2 != data->targx
		||  obj_centery (data->target) >> FRAC * 2 != data->targy)
		{
			data->targx = obj_centerx (data->target) >> FRAC * 2;
			data->targy = obj_centery (data->target) >> FRAC * 2;
			data->nodeidx = 1; // exclude first node since we're on it anyway
	
			if (data->nodelist)
				free (data->nodelist);
	
			data->nodelist = ai_get_path (obj, data->target, level);
			if (!data->nodelist)
				return;
		}
	
		distx = ((data->nodelist [data->nodeidx].x << FRAC * 2) + (8 << FRAC)) - obj_centerx (obj);
		disty = ((data->nodelist [data->nodeidx].y << FRAC * 2) + (8 << FRAC)) - obj_centery (obj);

		if (distx || disty)
		{
			obj->deltax = fixdiv (distx, abs (max (abs (distx), abs (disty)))) / 2;
			obj->deltay = fixdiv (disty, abs (max (abs (distx), abs (disty)))) / 2;

			obj_point (obj);
	
			obj->deltax = fixmul (obj->deltax, float_to_fixed ((xrand () % 40) / 20.0));
			obj->deltay = fixmul (obj->deltay, float_to_fixed ((xrand () % 40) / 20.0));
		}
	
		if (abs (distx) <= 3 << FRAC && abs (disty) <= 3 << FRAC
		&& (data->nodelist [data->nodeidx].x != (uint8)data->targx || data->nodelist [data->nodeidx].y != (uint8)data->targy))
		{
			data->nodeidx ++;
		}
	}
	else if (data->state == ai_attack)
	{
		fixed thres = float_to_fixed (23.0); // distance required
		distx = data->targx - obj_centerx (obj);
		disty = data->targy - obj_centery (obj);

		if (distx || disty)
		{
			obj->deltax = fixdiv (distx, thres);
			obj->deltay = fixdiv (disty, thres);

			obj_point (obj);
		}

		if (obj->frame == slime_anim_crawl1) // attack over
			data->state = ai_chase;
	}

	// in range of player?
	if (abs (obj_centerx (data->target) - obj_centerx (obj)) < 24 << FRAC
	&& abs (obj_centery (data->target) - obj_centery (obj)) < 24 << FRAC
	&& data->state != ai_attack)
	{
		obj_set_frame (obj, slime_anim_attack1);
		data->targx = obj_centerx (data->target);
		data->targy = obj_centery (data->target);
		data->state = ai_attack;
	}

	obj->x += obj->deltax;
	obj->y += obj->deltay;
	obj_collide_tiles (obj, level->tiles, level->w);
	obj->deltax = obj->deltay = 0;

	return;
}

/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "int.h"
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

	opened [iopen++] = &(nodelist [((obj->hitbox.x + obj->hitbox.w / 2) / 16) * l->w + ((obj->hitbox.y + obj->hitbox.h / 2) / 16)]);
	opened [iopen - 1]->type = N_OPENED;

	goal = &nodelist [((targ->hitbox.x + targ->hitbox.w / 2) / 16) * l->w + ((targ->hitbox.y + targ->hitbox.h / 2) / 16)];

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

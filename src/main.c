/* See LICENSE file for copyright and licensing details */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,0)
#error "Need SDL 2.0"
#endif

#include "int.h"
#include "xor.h"
#include "anim.h"
#include "obj.h"
#include "player.h"
#include "level.h"
#include "tile.h"
#include "rndr.h"
#include "input.h"
#include "ai.h"

level_t *level = NULL;

SDL_Window *win = NULL;
SDL_Renderer *rndr = NULL;

static uint16 ticktime = 1000 / 60;
SDL_Texture *fonttex;
int8 running = 1;
uint32 curtick = 0;
int8 renderlights = 1, renderdbg = 0, editmode = 0; // debug stuff, yay
obj_t *player;
SDL_Texture *torchtex;
SDL_Texture *hudbars, *hfill, *mfill;

SDL_Texture *tiletex;
SDL_Texture *tiletarg;
void update_tiles (void)
{
	int32 i, j;
	SDL_Rect tilesrc = { .w = 16, .h = 16 };
	SDL_Rect tiledst = { .w = 16, .h = 16 };

	SDL_SetRenderTarget (rndr, tiletarg);
	SDL_SetRenderDrawColor (rndr, 0, 0, 0, 255);
	SDL_RenderClear (rndr);
	for (i = 0; i < level->w; i++)
		for (j = 0; j < level->h; j++)
		{
			level->tiles [i * level->w + j].sheet = tiletex;
			level->tiles [i * level->w + j].offs = dungeon_tileoffs [level->offs [i * level->w + j]];

			// set to solid if this isn't a floor or if this is a border tile
			if ((level->offs [i * level->w + j] >= dun_wall_n && level->offs [i * level->w + j] <= dun_wall_osw + 5)
			|| level->offs [i * level->w + j] == dun_black || i == 0 || i == level->w - 1 || j == 0 || j == level->h - 1)
			{
				level->tiles [i * level->w + j].flags |= TF_SOLID;
				level->tiles [i * level->w + j].hitbox.x = i << FRAC * 2;
				level->tiles [i * level->w + j].hitbox.y = j << FRAC * 2;
				level->tiles [i * level->w + j].hitbox.w = level->tiles [i * level->w + j].hitbox.h = 16 << FRAC;
			}
			else if (level->offs [i * level->w + j] >= dun_stair_n && level->offs [i * level->w + j] <= dun_stair_w + 3)
			{
				level->tiles [i * level->w + j].flags |= TF_STAIR;
				level->tiles [i * level->w + j].flags &= ~TF_SOLID;
				level->tiles [i * level->w + j].hitbox.w = level->tiles [i * level->w + j].hitbox.h = 0;
			}
			else
			{
				level->tiles [i * level->w + j].flags &= ~(TF_SOLID | TF_STAIR);
				level->tiles [i * level->w + j].hitbox.w = level->tiles [i * level->w + j].hitbox.h = 0;
			}

			tilesrc.x = level->tiles [i * level->w + j].offs.x * 16;
			tilesrc.y = level->tiles [i * level->w + j].offs.y * 16;
			tiledst.x = i * 16;
			tiledst.y = j * 16;
			SDL_RenderCopy (rndr, level->tiles [i * level->w + j].sheet, &tilesrc, &tiledst);
		}

	SDL_SetRenderTarget (rndr, NULL);
}

int main (int argc, char **argv)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "SDL_Init failed (%s)\n", SDL_GetError ());
		return 1;
	}

	if (!(win = SDL_CreateWindow ("nemesis (" __DATE__ ")", SDL_WINDOWPOS_CENTERED,
	                              SDL_WINDOWPOS_CENTERED, 512, 320, 0)))
//	                              SDL_WINDOWPOS_CENTERED, 256, 160, 0)))
	{
		fprintf (stderr, "SDL_CreateWindow failed (%s)\n", SDL_GetError ());
		return 1;
	}

	if (!(rndr = SDL_CreateRenderer (win, -1, SDL_RENDERER_ACCELERATED)))
	{
		fprintf (stderr, "SDL_CreateRenderer failed (%s)\n", SDL_GetError ());
		return 1;
	}

	// initialize internal random state
	xsrand (time (NULL));

	SDL_Texture *screen = SDL_CreateTexture (rndr, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 256, 160);
	SDL_Rect camera = { .w = 256, .h = 160 };

	// testing crap now :D
	uint8 playercolor = (xrand () % 8) * 32;
	nif_t *plsprite = rndr_nif_load ("img/objects/player/male/male.nif");
	rndr_nif_shift (plsprite, 0, 160, 0, 0);
	rndr_nif_shift (plsprite, 1, 20, 0, -50);
	rndr_nif_shift (plsprite, 2, playercolor, 0, -30);
	rndr_nif_shift (plsprite, 3, playercolor, 0, -30);
	nif_t *torchspr = rndr_nif_load ("img/objects/dungeon/adungeon/torches.nif");
	nif_t *slimespr = rndr_nif_load ("img/objects/enemy/slime_tiny.nif");
	rndr_nif_shift (slimespr, 0, 32, -20, 0);
	nif_t *tilesheet = rndr_nif_load ("img/tiles/dungeon/adungeon.nif");
	torchtex = SDL_CreateTextureFromSurface (rndr, torchspr->sur);
	nif_t *hudbarspr = rndr_nif_load ("img/gui/bar.nif");
	nif_t *hudfillspr = rndr_nif_load ("img/gui/barfrag.nif");
	hudbars = SDL_CreateTextureFromSurface (rndr, hudbarspr->sur);
	rndr_nif_shift (hudfillspr, 0, 247, 255, 0);
	hfill = SDL_CreateTextureFromSurface (rndr, hudfillspr->sur);
	rndr_nif_reset (hudfillspr);
	rndr_nif_shift (hudfillspr, 0, 143, 255, 0);
	mfill = SDL_CreateTextureFromSurface (rndr, hudfillspr->sur);
	player = obj_create (64 << FRAC, 128 << FRAC, SDL_CreateTextureFromSurface (rndr, plsprite->sur), &char_anim, char_anim_idle1, ROT_DOWNRIGHT, player_thinker, NULL);
	obj_set_hitbox (player, 8 << FRAC, 16 << FRAC, 16 << FRAC, 16 << FRAC);

	int i;

	for (i = 0; i < 16; i++)
	{
		obj_t *slime;
		rndr_nif_reset (slimespr);
		rndr_nif_shift (slimespr, 0, xrand () % 256, xrand () % 256, ((int16)(xrand () % 128)) - 64);
		slime = obj_create (24 + (i % 8) * 20 << FRAC, 16 << FRAC, SDL_CreateTextureFromSurface (rndr, slimespr->sur), &slime_anim, 0, ROT_DOWNRIGHT, ai_thinker, NULL);
		obj_set_hitbox (slime, 4 << FRAC, 8 << FRAC, 8 << FRAC, 8 << FRAC);

		slime->data = malloc (sizeof (aidata_t));
		if (!slime->data)
			obj_destroy (slime);

		((aidata_t*)slime->data)->target = player;
		((aidata_t*)slime->data)->targx = ((aidata_t*)slime->data)->targy = 0;
		((aidata_t*)slime->data)->nodelist = NULL;
		((aidata_t*)slime->data)->nodeidx = 0;
		((aidata_t*)slime->data)->thinker = NULL;
	}

	if (argc < 2)
	{
		level = level_new (24, 24, NULL);
		rndr_add_light (0, 0, 0, 0, 48, 0, 0); // ambient lighting
	}
	else
	{
		level = level_load (argv [1]);
		if (!level)
		{
			printf ("failed load\n");
			level = level_new (24, 24, NULL);
			rndr_add_light (0, 0, 0, 0, 48, 0, 0); // ambient lighting
		}
	}


	if (!level)
		return 1;

	tiletex = SDL_CreateTextureFromSurface (rndr, tilesheet->sur);
	tiletarg = SDL_CreateTexture (rndr, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, level->w * 16, level->h * 16);

	update_tiles ();

	// font
	nif_t *fontspr = rndr_nif_load ("img/gui/font.nif");
	fonttex = SDL_CreateTextureFromSurface (rndr, fontspr->sur);

	SDL_SetRenderDrawBlendMode (rndr, SDL_BLENDMODE_MOD);

	SDL_Event ev;

	uint16 frametimes [48] = { 0 };
	int32 nexttick = SDL_GetTicks ();

	while (running)
	{
		static int32 sleeptime, loops;

		// Set tick
		curtick ++;
		nexttick += ticktime;

		// Get input, run object logic code
		input_get (&ev);

		obj_do_advframes ();
		obj_do_thinkers ();

		if (SDL_GetTicks () > nexttick && loops++ < 10)
			continue;

		loops = 0;

		// Clear the screen, set draw target to the non-scaled 256x160 texture
		SDL_SetRenderDrawColor (rndr, 0, 0, 0, 255);
		SDL_RenderClear (rndr);
		SDL_SetRenderTarget (rndr, screen);

		// Render
		rndr_do_camera (&camera, player, level->w * 16, level->h * 16);
		rndr_do_tiles (tiletarg, &camera);
		rndr_do_objs (&camera);

		if (renderlights)
			rndr_do_lighting (&camera, level);

		if (renderdbg)
			rndr_do_debug (frametimes, &camera, player);

		if (editmode)
			rndr_do_edithud (&camera, level, obj_centerx (player) >> FRAC * 2, obj_centery (player) >> FRAC * 2);

		if (!editmode && !renderdbg)
			rndr_do_hud ();

		// Set drawing target to the scaled texture, and copy to it
		SDL_SetRenderTarget (rndr, NULL);
		SDL_RenderCopy (rndr, screen, NULL, NULL);

		SDL_RenderPresent (rndr);

		// Calculate total frame time, sleep until next frame
		sleeptime = nexttick - SDL_GetTicks ();
		if (sleeptime > 0)
			SDL_Delay (sleeptime);

		frametimes [curtick % 48] = ticktime - sleeptime; 
	}

	SDL_DestroyRenderer (rndr);
	SDL_DestroyWindow (win);
	SDL_Quit ();
	return 0;
}

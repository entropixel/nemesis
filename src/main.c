/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,0)
#error "Need SDL 2.0"
#endif

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "player.h"
#include "tile.h"
#include "rndr.h"
#include "input.h"

tile_t levtiles [16] [17];

int32 levtiles_offs [17] [16] = // FUCK :(
{
	{ dun_wall_inw + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2,
	  dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2, dun_wall_n + 2,
	  dun_wall_n + 2, dun_wall_ine + 2 },
	{ dun_wall_w + 2, dun_wall_inw + 1, dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1,
	  dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1, dun_wall_n + 1,
	  dun_wall_ine + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_inw, dun_wall_n, dun_wall_n, dun_wall_n, dun_wall_n, dun_wall_n, dun_wall_n,
	  dun_wall_n, dun_wall_n, dun_wall_n, dun_wall_n, dun_wall_ine, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_w, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor, dun_floor,
	  dun_floor, dun_floor, dun_floor, dun_wall_e, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_w + 1, dun_wall_isw, dun_wall_s, dun_wall_s, dun_wall_s, dun_wall_s, dun_wall_s, dun_wall_s,
	  dun_wall_s, dun_wall_s, dun_wall_s, dun_wall_s, dun_wall_ise, dun_wall_e + 1, dun_wall_e + 2 },
	{ dun_wall_w + 2, dun_wall_isw + 1, dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1,
	  dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1, dun_wall_s + 1,
	  dun_wall_ise + 1, dun_wall_e + 2 },
	{ dun_wall_isw + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2,
	  dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2, dun_wall_s + 2,
	  dun_wall_s + 2, dun_wall_ise + 2 }
};

SDL_Window *win = NULL;
SDL_Renderer *rndr = NULL;

light_t torch2 =
{
	1, 6,
	28, 100, 142, 24, 10,
	NULL
};

light_t torch1 =
{
	8, 1,
	28, 100, 142, 24, 10,
	&torch2
};

light_t ambience =
{
	0, 0,
	0, 0, 48, 0, 0,
	&torch1
};

static uint16 ticktime = 1000 / 60;
SDL_Texture *fonttex;
int8 running = 1;
uint32 curtick = 0;
int8 renderlights = 1, renderdbg = 0, editmode = 0; // debug stuff, yay

void update_tiles (SDL_Texture *tiletarg, SDL_Texture *tiletex)
{
	int32 i, j;
	SDL_Rect tilesrc = { .w = 16, .h = 16 };
	SDL_Rect tiledst = { .w = 16, .h = 16 };

	SDL_SetRenderTarget (rndr, tiletarg);
	for (i = 0; i < 16; i++)
		for (j = 0; j < 17; j++)
		{
			levtiles [i] [j].sheet = tiletex;
			levtiles [i] [j].offs = dungeon_tileoffs [levtiles_offs [j] [i]];

			// set to solid if this isn't a floor
			if (levtiles_offs [j] [i])
				levtiles [i] [j].flags |= TF_SOLID;

			tilesrc.x = levtiles [i] [j].offs.x * 16;
			tilesrc.y = levtiles [i] [j].offs.y * 16;
			tiledst.x = i * 16;
			tiledst.y = j * 16;
			SDL_RenderCopy (rndr, levtiles [i] [j].sheet, &tilesrc, &tiledst);
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
	{
		fprintf (stderr, "SDL_CreateWindow failed (%s)\n", SDL_GetError ());
		return 1;
	}

	if (!(rndr = SDL_CreateRenderer (win, -1, SDL_RENDERER_ACCELERATED)))
	{
		fprintf (stderr, "SDL_CreateRenderer failed (%s)\n", SDL_GetError ());
		return 1;
	}

	SDL_Texture *screen = SDL_CreateTexture (rndr, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 256, 160);
	SDL_Rect camera = { .w = 260, .h = 160 };

	// testing crap now :D
	nif_t *plsprite = rndr_nif_load ("img/objects/player/male/male.nif");
	rndr_nif_shift (plsprite, 0, 160, 0, 0);
	rndr_nif_shift (plsprite, 1, 20, 0, -50);
	rndr_nif_shift (plsprite, 2, 77, 0, -30);
	rndr_nif_shift (plsprite, 3, 77, 0, -30);
	nif_t *torchspr = rndr_nif_load ("img/objects/dungeon/adungeon/torches.nif");
	nif_t *tilesheet = rndr_nif_load ("img/tiles/dungeon/adungeon.nif");
	obj_t *torch = obj_create (128, 16, SDL_CreateTextureFromSurface (rndr, torchspr->sur), &torch_anim, 0, ROT_DOWN, NULL);
	obj_t *torchb = obj_create (16, 96, torch->tex, &torch_anim, 0, ROT_RIGHT, NULL);
	obj_t *player = obj_create (64, 64, SDL_CreateTextureFromSurface (rndr, plsprite->sur), &char_anim, char_anim_idle1, ROT_DOWNRIGHT, player_thinker);
	obj_set_hitbox (player, 8, 16, 16, 16);

	// for now, render the level tiles to a seperate SDL_Texture, to speed things up
	SDL_Texture *tiletex = SDL_CreateTextureFromSurface (rndr, tilesheet->sur);
	SDL_Texture *tiletarg = SDL_CreateTexture (rndr, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 16 * 16, 17 * 16);

	update_tiles (tiletarg, tiletex);

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
		rndr_do_camera (&camera, player, 16 * 16, 17 * 16);
		rndr_do_tiles (tiletarg, &camera);
		rndr_do_objs (&camera);

		if (renderlights)
			rndr_do_lighting (&ambience, &camera, 16, 17);

		if (renderdbg)
			rndr_do_debug (frametimes, &camera, player);

		if (editmode)
			rndr_do_edithud ();

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

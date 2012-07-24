/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#if !SDL_VERSION_ATLEAST(2,0,0)
#error "Need SDL 2.0"
#endif

#include "anim.h"
#include "obj.h"
#include "tile.h"
#include "rndr.h"

tile_t levtiles [16] [10];

int levtiles_offs [10] [16] = // FUCK :(
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

enum
{
	UPK = 1,
	RIGHTK = 2,
	DOWNK = 4,
	LEFTK = 8
};

unsigned int keymask = 0;

// alphas: 
// hair - 232
// eyes - 242
// shirt - 237
// pants - 235

light_t torch2 =
{
	1, 6,
	28, 200, 180, 30,
	NULL
};

light_t torch1 =
{
	8, 1,
	28, 200, 180, 30,
	&torch2
};

light_t ambience =
{
	0, 0,
	0, 0, 30, 0,
	&torch1
};

SDL_Texture *fonttex;

int main (int argc, char **argv)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "SDL_Init failed (%s)\n", SDL_GetError ());
		return 1;
	}

	if (!(IMG_Init (IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		fprintf (stderr, "IMG_Init failed (%s)\n", IMG_GetError ());
		return 1;
	}

	if (!(win = SDL_CreateWindow ("nemesis (" __DATE__ ")", SDL_WINDOWPOS_CENTERED,
	                              SDL_WINDOWPOS_CENTERED, 256, 160, 0)))
	{
		fprintf (stderr, "SDL_CreateWindow failed (%s)\n", SDL_GetError ());
		return 1;
	}

	if (!(rndr = SDL_CreateRenderer (win, -1, SDL_RENDERER_ACCELERATED)))
	{
		fprintf (stderr, "SDL_CreateRenderer failed (%s)\n", SDL_GetError ());
		return 1;
	}

	// testing crap now :D
	SDL_Surface *plsprite = IMG_Load ("img/objects/player/male/nosleeve_shorts_short.png");
	rndr_shift_sprite (plsprite, 232, 20, 0, -50);
	rndr_shift_sprite (plsprite, 242, 160, 0, 0);
	rndr_shift_sprite (plsprite, 237, 77, 0, -30);
	rndr_shift_sprite (plsprite, 235, 80, 0, -30);
	SDL_Surface *torchspr = IMG_Load ("img/objects/dungeon/adungeon/torches.png");
	SDL_Surface *tilesheet = IMG_Load ("img/tiles/dungeon/adungeon.png");
	obj_t *player = obj_create (32, 32, SDL_CreateTextureFromSurface (rndr, plsprite), &char_anim, char_anim_stand, ROT_DOWNRIGHT);
	obj_t *torch = obj_create (128, 16, SDL_CreateTextureFromSurface (rndr, torchspr), &torch_anim, 0, ROT_DOWN);
	obj_t *torchb = obj_create (16, 96, torch->tex, &torch_anim, 0, ROT_RIGHT);

	// for now, render the level tiles to a seperate SDL_Texture, to speed things up
	SDL_Texture *tiletex = SDL_CreateTextureFromSurface (rndr, tilesheet);
	SDL_Texture *tiletarg = SDL_CreateTexture (rndr, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 256, 160);

	{
		int i, j;
		SDL_Rect tilesrc = { .w = 16, .h = 16 };
		SDL_Rect tiledst = { .w = 16, .h = 16 };

		SDL_SetRenderTarget (rndr, tiletarg);
		for (i = 0; i < 16; i++)
			for (j = 0; j < 10; j++)
			{
				levtiles [i] [j].sheet = tiletex;
				levtiles [i] [j].offs = dungeon_tileoffs [levtiles_offs [j] [i]];

				tilesrc.x = levtiles [i] [j].offs.x * 16;
				tilesrc.y = levtiles [i] [j].offs.y * 16;
				tiledst.x = i * 16;
				tiledst.y = j * 16;
				SDL_RenderCopy (rndr, levtiles [i] [j].sheet, &tilesrc, &tiledst);
			}

		SDL_SetRenderTarget (rndr, NULL);
	}

	// font
	SDL_Surface *fontspr = IMG_Load ("img/gui/font.png");
	SDL_Rect toptxtrct, bottxtrct;
	fonttex = SDL_CreateTextureFromSurface (rndr, fontspr);
	SDL_Texture *titletxt = rndr_make_text (rndr, "Nemesis PreAlpha", &toptxtrct);
	SDL_Texture *vertxt = rndr_make_text (rndr, __DATE__, &bottxtrct);
	toptxtrct.x = toptxtrct.y = bottxtrct.x = 8;
	bottxtrct.y = 24;

	SDL_SetRenderDrawBlendMode (rndr, SDL_BLENDMODE_MOD);

	SDL_Event ev;

	char renderlights = 1, renderdbg = 0; // debug stuff, yay
	unsigned short frametimes [48] = { 0 };

	char running = 1;
	unsigned int curtick = 0;
	while (running)
	{
		int preticks = SDL_GetTicks (), postticks;

		while (SDL_PollEvent (&ev))
		{
			switch (ev.type)
			{
				case SDL_KEYUP:
					switch (ev.key.keysym.sym)
					{
						case SDLK_UP:
							keymask &= ~(UPK);
							break;
						case SDLK_RIGHT:
							keymask &= ~(RIGHTK);
							break;
						case SDLK_DOWN:
							keymask &= ~(DOWNK);
							break;
						case SDLK_LEFT:
							keymask &= ~(LEFTK);
							break;
						case SDLK_F2:
							renderlights = !renderlights;
							break;
						case SDLK_F3:
							renderdbg = !renderdbg;
							break;
					}
				break;
				case SDL_KEYDOWN:
					switch (ev.key.keysym.sym)
					{
						case SDLK_UP:
							keymask |= UPK;
							break;
						case SDLK_RIGHT:
							keymask |= RIGHTK;
							break;
						case SDLK_DOWN:
							keymask |= DOWNK;
							break;
						case SDLK_LEFT:
							keymask |= LEFTK;
							break;
					}
				break;
				case SDL_QUIT:
					running = 0;
				break;
			}
		}

		if (keymask)
		{
			switch (keymask)
			{
				case UPK:
					obj_set_rot (player, ROT_UP);
					player->y -= 1.41f;
				break;
				case UPK | RIGHTK:
					obj_set_rot (player, ROT_UPRIGHT);
					player->x += 1.0f;
					player->y -= 1.0f;
				break;
				case RIGHTK:
					obj_set_rot (player, ROT_RIGHT);
					player->x += 1.41f;
				break;
				case RIGHTK | DOWNK:
					obj_set_rot (player, ROT_DOWNRIGHT);
					player->x += 1.0f;
					player->y += 1.0f;
				break;
				case DOWNK:
					obj_set_rot (player, ROT_DOWN);
					player->y += 1.41f;
				break;
				case DOWNK | LEFTK:
					obj_set_rot (player, ROT_DOWNLEFT);
					player->x -= 1.0f;
					player->y += 1.0f;
				break;
				case LEFTK:
					obj_set_rot (player, ROT_LEFT);
					player->x -= 1.41f;
				break;
				case LEFTK | UPK:
					obj_set_rot (player, ROT_UPLEFT);
					player->x -= 1.0f;
					player->y -= 1.0f;
				break;
				default:
				break;
			}

			if (player->frame == char_anim_stand)
				obj_set_frame (player, char_anim_walk1);

			player->dest.x = (short) player->x;
			player->dest.y = (short) player->y;
		}
		else
			obj_set_frame (player, char_anim_stand);

		SDL_SetRenderDrawColor (rndr, 0, 0, 0, 255);
		SDL_RenderClear (rndr);

		SDL_RenderCopy (rndr, tiletarg, NULL, NULL);

		SDL_RenderCopy (rndr, torch->tex, &(torch->show), &(torch->dest));
		SDL_RenderCopy (rndr, torchb->tex, &(torchb->show), &(torchb->dest));
		SDL_RenderCopy (rndr, player->tex, &(player->show), &(player->dest));

		if (renderlights)
			rndr_do_lighting (rndr, &ambience);

		SDL_SetRenderDrawBlendMode (rndr, SDL_BLENDMODE_NONE);
		int i;
		for (i = 0; i < 48 && renderdbg; i++)
		{
			SDL_Rect rtime = { .x = i * 2, .w = 2, .h = frametimes [i] * 2 };
			rtime.y = 160 - rtime.h;

			SDL_SetRenderDrawColor (rndr, (frametimes [i] > 8) ? 255 : 0, (frametimes [i] < 17) ? 255 : 0, 0, 128);
			SDL_RenderFillRect (rndr, &rtime);
			if (!i)
			{
				SDL_RenderCopy (rndr, titletxt, NULL, &toptxtrct);
				SDL_RenderCopy (rndr, vertxt, NULL, &bottxtrct);
			}
		}
		SDL_SetRenderDrawBlendMode (rndr, SDL_BLENDMODE_MOD);

		SDL_RenderPresent (rndr);

		obj_adv_frame (player);
		obj_adv_frame (torch);
		obj_adv_frame (torchb);

		postticks = SDL_GetTicks ();
		frametimes [curtick % 48] = postticks - preticks; 

		if (postticks - preticks < (1000/60))
			SDL_Delay ((1000/60) - (postticks - preticks));

		curtick ++;
	}

	SDL_DestroyRenderer (rndr);
	SDL_DestroyWindow (win);
	IMG_Quit ();
	SDL_Quit ();
	return 0;
}

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

tile_t levtiles [16] [10];
int levtiles_offs [16] [10] = // FUCK
{
	{ 26, 6, 6, 6, 6, 6, 6, 6, 6, 18 },
	{ 10, 25, 5, 5, 5, 5, 5, 5, 17, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 9, 0, 0, 0, 0, 0, 0, 1, 2 },
	{ 10, 29, 13, 13, 13, 13, 13, 13, 21, 2 },
	{ 30, 14, 14, 14, 14, 14, 14, 14, 14, 22 }
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
// pants - 236

void rndr_rgb_to_hsl (unsigned char *res, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char h, s, l;
	unsigned char max = (((r > g) ? r : g) > b) ? ((r > g) ? r : g) : b;
	unsigned char min = (((r < g) ? r : g) < b) ? ((r < g) ? r : g) : b;
	unsigned char del = max - min;
	
	l = (min + max) / 2;
	s = 0;

	if (l > 0 && l < 255)
		s = (unsigned char)(((unsigned int)255 * del) / ((l > 128) ? (512 - del) : (max + min)));

	if (del)
	{
		if (max == r)
			h = 43 * (g - b) / del;
		else if (max == g)
			h = 85 + (43 * (b - r) / del);
		else
			h = 171 + (43 * (r - g) / del);
	}

	res [0] = h;
	res [1] = s;
	res [2] = l;

	return;
}

#define HSLMUL(x,y) ((((unsigned int)(x)) * ((unsigned int)(y))) / 256)
#define HSLDIV(x,y) ((((unsigned int)(x)) * 256) / ((unsigned int)(y)))

void rndr_hsl_to_rgb (unsigned char *res, unsigned char h, unsigned char s, unsigned char l)
{
	unsigned int sr, sg, sb, tmpr, tmpg, tmpb;
	register unsigned int hh, ss, ll;

	hh = h;
	ss = s;
	ll = l;

	if (hh < 85)
	{
		sr = HSLDIV (85 - hh, 43);
		sg = HSLDIV (hh, 43);
		sb = 0;
	}
	else if (hh < 171)
	{
		sr = 0;
		sg = HSLDIV (171 - hh, 43);
		sb = HSLDIV (hh - 85, 43);
	}
	else
	{
		sr = HSLDIV (hh - 171, 43);
		sg = 0;
		sb = HSLDIV (256 - hh, 43);
	}

	sr = (sr < 256) ? sr : 256;
	sg = (sg < 256) ? sg : 256;
	sb = (sb < 256) ? sb : 256;

	tmpr = HSLMUL (512, HSLMUL (ss, sr)) + (256 - ss);
	tmpg = HSLMUL (512, HSLMUL (ss, sg)) + (256 - ss);
	tmpb = HSLMUL (512, HSLMUL (ss, sb)) + (256 - ss);

	if (ll < 128)
	{
		res[0] = HSLMUL (ll, tmpr);
		res[1] = HSLMUL (ll, tmpg);
		res[2] = HSLMUL (ll, tmpb);
	}
	else
	{
		res[0] = HSLMUL (256 - ll, tmpr) + HSLMUL (512, ll - 256);
		res[1] = HSLMUL (256 - ll, tmpg) + HSLMUL (512, ll - 256);
		res[2] = HSLMUL (256 - ll, tmpb) + HSLMUL (512, ll - 256);
	}

	return;
}

// shifts all pixels in the sprite that have the specified alpha value
void rndr_shift_sprite (SDL_Surface *spr, unsigned char alpha, char shift)
{
	SDL_PixelFormat *f = spr->format;
	int i, j;

	for (i = 0; i < spr->w; i++)
		for (j = 0; j < spr->h; j++)
		{
			unsigned int *pix = spr->pixels + (j * spr->w * f->BytesPerPixel) + (i * f->BytesPerPixel);

			if ((*pix & f->Amask) >> f->Ashift == alpha)
			{
				unsigned char hsl [3];
				unsigned char rgb [3];
				
				// convert pixel's rgb to hsl, shift, and reconvert
				rndr_rgb_to_hsl (hsl, (*pix & f->Rmask) >> f->Rshift, (*pix & f->Gmask) >> f->Gshift, (*pix & f->Bmask) >> f->Bshift);
				hsl [0] += shift; // shift
				rndr_hsl_to_rgb (rgb, hsl [0], hsl [1], hsl [2]);
				*pix = (rgb [0] << f->Rshift) | (rgb [1] << f->Gshift) | (rgb [2] << f->Bshift) | (255 << f->Ashift);
			} 
		}

	return;
}

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
	rndr_shift_sprite (plsprite, 232, 20);
/*	rndr_shift_sprite (plsprite, 242, 110);
	rndr_shift_sprite (plsprite, 237, 85);
	rndr_shift_sprite (plsprite, 236, 90);
*/	SDL_Surface *torchspr = IMG_Load ("img/objects/dungeon/adungeon/torches.png");
	SDL_Surface *tilesheet = IMG_Load ("img/tiles/dungeon/adungeon.png");
	obj_t *player = obj_create (32, 32, SDL_CreateTextureFromSurface (rndr, plsprite), &char_anim, char_anim_stand, ROT_DOWNRIGHT);
	obj_t *torch = obj_create (128, 16, SDL_CreateTextureFromSurface (rndr, torchspr), &torch_anim, 0, ROT_DOWN);
	SDL_Texture *tiletex = SDL_CreateTextureFromSurface (rndr, tilesheet);
	SDL_SetRenderDrawBlendMode (rndr, SDL_BLENDMODE_MOD);

	// ew.
	int i, j;
	for (i = 0; i < 16; i++)
		for (j = 0; j < 10; j++)
		{
			levtiles [i] [j].sheet = tiletex;
			levtiles [i] [j].offs = dungeon_tileoffs [levtiles_offs [i] [j]];
		}

	SDL_Event ev;

	char running = 1;
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

		SDL_Rect tilerct = { .w = 16, .h = 16 };
		SDL_Rect tiledst = { .w = 16, .h = 16 };
		for (i = 0; i < 16; i++)
			for (j = 0; j < 10; j++)
			{
				tilerct.x = levtiles [i] [j].offs.x * 16;
				tilerct.y = levtiles [i] [j].offs.y * 16;
				tiledst.x = i * 16;
				tiledst.y = j * 16;
				SDL_RenderCopy (rndr, levtiles [i] [j].sheet, &tilerct, &tiledst);
			}

		SDL_RenderCopy (rndr, torch->tex, &(torch->show), &(torch->dest));
		SDL_RenderCopy (rndr, player->tex, &(player->show), &(player->dest));
		for (i = 0; i < 16; i++)
			for (j = 0; j < 10; j++)
			{
				int sub = 30 * (abs (i - 8) + abs (j - 1)) + torch->frame, col;
				tiledst.x = i * 16;
				tiledst.y = j * 16;
				col = (sub > 175) ? 50 : 225 - sub;
				SDL_SetRenderDrawColor (rndr, col + 20, col + 10, col, 255);
				SDL_RenderFillRect (rndr, &tiledst);
			}

		SDL_RenderPresent (rndr);

		obj_adv_frame (player);
		obj_adv_frame (torch);

		postticks = SDL_GetTicks ();
		if (postticks - preticks < (1000/60))
			SDL_Delay ((1000/60) - (postticks - preticks));
	}

	SDL_DestroyRenderer (rndr);
	SDL_DestroyWindow (win);
	IMG_Quit ();
	SDL_Quit ();
	return 0;
}

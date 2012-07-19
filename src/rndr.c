/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include "rndr.h"

static inline void rndr_rgb_to_hsl (unsigned char *res, unsigned char r, unsigned char g, unsigned char b)
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

static inline void rndr_hsl_to_rgb (unsigned char *res, unsigned char h, unsigned char s, unsigned char l)
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
void rndr_shift_sprite (SDL_Surface *spr, unsigned char alpha, char hshift, char sshift, char lshift)
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
				hsl [0] += hshift;
				hsl [1] += sshift;
				hsl [2] += lshift;
				rndr_hsl_to_rgb (rgb, hsl [0], hsl [1], hsl [2]);
				*pix = (rgb [0] << f->Rshift) | (rgb [1] << f->Gshift) | (rgb [2] << f->Bshift) | (255 << f->Ashift);
			} 
		}

	return;
}

extern SDL_Texture *fonttex;
SDL_Texture *rndr_make_text (SDL_Renderer *rndr, const char *text, SDL_Rect *inf)
{
	int i;
	SDL_Texture *ret = SDL_CreateTexture (rndr, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, strlen (text) * 8, 8);
	SDL_SetTextureBlendMode (ret, SDL_BLENDMODE_ADD);
	SDL_SetRenderTarget (rndr, ret);

	for (i = 0; i < strlen (text); i++)
	{
		SDL_Rect src = { (text [i] & 0x0f) * 8, ((text [i] & 0xf0) >> 4) * 8, 8, 8 };
		SDL_Rect dst = { i * 8, 0, 8, 8 };

		SDL_RenderCopy (rndr, fonttex, &src, &dst);
	}

	if (inf) // give them some info about the text if we want it
	{
		inf->h = 8;
		inf->w = strlen (text) * 8;
	}

	SDL_SetRenderTarget (rndr, NULL);
	return ret;
}

#define min(a,b) ((a < b) ? a : b)
void rndr_do_lighting (SDL_Renderer *rndr, light_t *l)
{
	// assume 16x10 tiles at 16x16 pixels each
	int i, j; 
	unsigned char rgb [3];
	int rsum, gsum, bsum, sub;
	light_t *it;
	SDL_Rect r = { .w = 16, .h = 16 };

	for (i = 0; i < 16; i++)
		for (j = 0; j < 10; j++)
		{
			rsum = gsum = bsum = 0;
			it = l;

			r.x = i * 16;
			r.y = j * 16;

			while (it)
			{
				sub = it->falloff * (abs (i - it->x) + abs (j - it->y));

				if (sub < 255)
				{
					rndr_hsl_to_rgb (rgb, it->hue, it->sat, (sub > it->bright) ? 0 : it->bright - sub);
					rsum += rgb [0];
					gsum += rgb [1];
					bsum += rgb [2];
				}

				it = it->next;
			}

			SDL_SetRenderDrawColor (rndr, min (255, rsum), min (255, gsum), min (255, bsum), 255);			
			SDL_RenderFillRect (rndr, &r);
		}

	return;
}

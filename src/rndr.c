/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <SDL.h>

#include "miniz.h"
#include "anim.h"
#include "obj.h"
#include "rndr.h"

#ifdef WIN32
#include <io.h>
#define set_binary_mode(f) setmode (f, O_BINARY)
#else
#define set_binary_mode(f)
#endif

extern SDL_Renderer *rndr;

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

nif_t *rndr_nif_load (const char *path)
{
	int fd, totalread = 0, tmpread, zret, i = 0;
	char id [4];
	unsigned short width, height;
	unsigned long imglen, zimglen;
	unsigned int zimglen32, numentr;
	unsigned char *img = NULL, *zimg = NULL;
	nif_t *ret = NULL;

	if ((fd = open (path, O_RDONLY)) < 0)
		goto rndr_nif_load_err;

	set_binary_mode (fd);

	read (fd, id, 4);
	if (strncmp (id, "nif\0", 4))
		goto rndr_nif_load_err;

	if (read (fd, &width, 2) != 2 || read (fd, &height, 2) != 2 || read (fd, &zimglen32, 4) != 4)
		goto rndr_nif_load_err;

	imglen = width * height * 4;
	zimglen = zimglen32;
	img = malloc (imglen);
	zimg = malloc (zimglen);
	if (!img || !zimg)
		goto rndr_nif_load_err;

	do
	{
		if ((tmpread = read (fd, zimg + totalread, zimglen - totalread)) < 0)
			break;
		else
			totalread += tmpread;
	} while (totalread < zimglen);

	if (totalread != zimglen)
		goto rndr_nif_load_err;

	if ((zret = uncompress (img, &imglen, zimg, zimglen)) != Z_OK)
	{
		fprintf (stderr, "rndr_nif_load: Error decompressing %s data (%s)\n", path, zError (zret));
		goto rndr_nif_load_err;
	}

	free (zimg);
	zimg = NULL;

	ret = malloc (sizeof (nif_t));
	if (!ret)
		goto rndr_nif_load_err;

	memset (ret, 0, sizeof (nif_t));

	ret->sur = SDL_CreateRGBSurfaceFrom (img, width, height, 32, width * 4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	// load shifts into tables
	while (read (fd, &numentr, 4) == 4 && i < 32)
	{
		int j;

		ret->shifts [i].numentr = numentr;
		ret->shifts [i].x = malloc (numentr * 2);
		ret->shifts [i].y = malloc (numentr * 2);

		for (j = 0; j < numentr; j++)
			if (read (fd, &(ret->shifts [i].x [j]), 2) != 2 || read (fd, &(ret->shifts [i].y [j]), 2) != 2)
				break;

		i ++;
	}

	return ret;

	rndr_nif_load_err:
	close (fd);
	free (img);
	free (zimg);

	if (ret && ret->sur)
		SDL_FreeSurface (ret->sur);

	for (i = 0; i < 32 && ret; i++)
	{
		free (ret->shifts [i].x);
		free (ret->shifts [i].y);
	}

	free (ret);
	return NULL;
}

// shifts all pixels in specified shift group
void rndr_nif_shift (nif_t *spr, int g, char hshift, char sshift, char lshift)
{
	SDL_PixelFormat *f = spr->sur->format;
	int i;

	for (i = 0; i < spr->shifts [g].numentr; i++)
	{
		unsigned int *pix = spr->sur->pixels + (spr->shifts [g].y [i] * spr->sur->w * f->BytesPerPixel) + (spr->shifts [g].x [i] * f->BytesPerPixel);

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

	return;
}

extern SDL_Texture *fonttex;
SDL_Texture *rndr_make_text (const char *text, SDL_Rect *inf)
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

void rndr_do_tiles (SDL_Texture *tiles) // todo, heh
{
	SDL_RenderCopy (rndr, tiles, NULL, NULL);
	return;
}

void rndr_do_objs (void)
{
	obj_t *it = obj_list_head;

	while (it)
	{
		SDL_RenderCopy (rndr, it->tex, &(it->show), &(it->dest));
		it = it->next;
	}

	return;
}

#define min(a,b) ((a < b) ? a : b)
void rndr_do_lighting (light_t *l)
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
				//sub = it->falloff * sqrt ((i - it->x) * (i - it->x) + (j - it->y) * (j - it->y));

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

void rndr_do_debug (unsigned short *frametimes)
{
	int i;
	static SDL_Rect toprct = { .x = 8, .y = 8 }, botrct = { .x = 8, .y = 24 };
	static SDL_Texture *titletxt = NULL, *vertxt = NULL;

	if (!titletxt && !vertxt)
	{
		titletxt = rndr_make_text ("Nemesis PreAlpha", &toprct);
		vertxt = rndr_make_text (__DATE__, &botrct);
	}

	SDL_SetRenderDrawBlendMode (rndr, SDL_BLENDMODE_NONE);
	for (i = 0; i < 48; i++)
	{
		SDL_Rect rtime = { .x = i * 2, .w = 2, .h = frametimes [i] * 2 };
		rtime.y = 160 - rtime.h;

		SDL_SetRenderDrawColor (rndr, (frametimes [i] > 8) ? 255 : 0, (frametimes [i] < 17) ? 255 : 0, 0, 128);
		SDL_RenderFillRect (rndr, &rtime);
		if (!i)
		{
			SDL_RenderCopy (rndr, titletxt, NULL, &toprct);
			SDL_RenderCopy (rndr, vertxt, NULL, &botrct);
		}
	}

	SDL_SetRenderDrawBlendMode (rndr, SDL_BLENDMODE_MOD);
	return;
}

/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef RNDR_H__
#define RNDR_H__

typedef struct light_s
{
	unsigned char x;
	unsigned char y;
	unsigned char hue;
	unsigned char sat;
	unsigned char bright;
	unsigned char falloff;
	struct light_s *next;
} light_t;

typedef struct
{
	SDL_Surface *sur;
	struct
	{
		unsigned int numentr;
		unsigned short *x;
		unsigned short *y;
	} shifts [32];
} nif_t;

nif_t *rndr_nif_load (const char *path);
void rndr_nif_shift (nif_t *spr, int g, char hshift, char sshift, char lshift);
SDL_Texture *rndr_make_text (const char *text, SDL_Rect *inf);
void rndr_do_tiles (SDL_Texture *tiles);
void rndr_do_objs (void);
void rndr_do_lighting (light_t *l);
void rndr_do_debug (unsigned short *frametimes);

#endif // RNDR_H__

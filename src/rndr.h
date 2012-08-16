/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef RNDR_H__
#define RNDR_H__

typedef struct light_s
{
	uint8 x;
	uint8 y;
	uint8 hue;
	uint8 sat;
	uint8 bright;
	uint8 falloff;
	uint8 flicker;
	struct light_s *next;
} light_t;

typedef struct
{
	SDL_Surface *sur;
	struct
	{
		uint32 numentr;
		uint16 *x;
		uint16 *y;
	} shifts [32];
} nif_t;

nif_t *rndr_nif_load (const char *path);
void rndr_nif_shift (nif_t *spr, int32 g, int16 hshift, int16 sshift, int16 lshift);
SDL_Texture *rndr_make_text (const char *text, SDL_Rect *inf);
void rndr_do_tiles (SDL_Texture *tiles);
void rndr_do_objs (void);
void rndr_do_lighting (light_t *l);
void rndr_do_debug (uint16 *frametimes);

#endif // RNDR_H__

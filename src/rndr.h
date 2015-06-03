/* See LICENSE file for copyright and licensing details */
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
	uint8 *img;
	uint8 *orig;
	SDL_Surface *sur;
	struct
	{
		uint32 numentr;
		uint16 *x;
		uint16 *y;
	} shifts [32];
} nif_t;

struct level_t;

nif_t *rndr_nif_load (const char *path);
void rndr_nif_shift (nif_t *spr, int32 g, int16 hshift, int16 sshift, int16 lshift);
void rndr_nif_reset (nif_t *spr);
SDL_Texture *rndr_make_text (const char *text, SDL_Rect *inf);
void rndr_do_camera (SDL_Rect *camera, obj_t *follow, int16 w, int16 h);
void rndr_do_tiles (SDL_Texture *tiles, SDL_Rect *camera);
void rndr_do_objs (SDL_Rect *camera);
void rndr_add_light (uint8 x, uint8 y, uint8 hue, uint8 sat, uint8 bright, uint8 falloff, uint8 flicker);
void rndr_clear_lights (void);
void rndr_do_lighting (SDL_Rect *camera, struct level_t *l);
void rndr_do_hud (void);
void rndr_do_debug (uint16 *frametimes, SDL_Rect *camera, obj_t *player);
void rndr_do_edithud (SDL_Rect *camera, struct level_t *l, uint8 selx, uint8 sely);

#endif // RNDR_H__

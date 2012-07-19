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

void rndr_shift_sprite (SDL_Surface *spr, unsigned char alpha, char hshift, char sshift, char lshift);
SDL_Texture *rndr_make_text (SDL_Renderer *rndr, const char *text);
void rndr_do_lighting (SDL_Renderer *rndr, light_t *l);

#endif // RNDR_H__

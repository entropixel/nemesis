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

void rndr_rgb_to_hsl (unsigned char *res, unsigned char r, unsigned char g, unsigned char b);
void rndr_hsl_to_rgb (unsigned char *res, unsigned char h, unsigned char s, unsigned char l);
void rndr_do_lighting (SDL_Renderer *rndr, light_t *l, unsigned char addr, unsigned char addg, unsigned char addb);

#endif // RNDR_H__

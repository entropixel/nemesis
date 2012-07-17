/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef RNDR_H__
#define RNDR_H__

void rndr_rgb_to_hsl (unsigned char *res, unsigned char r, unsigned char g, unsigned char b);
void rndr_hsl_to_rgb (unsigned char *res, unsigned char h, unsigned char s, unsigned char l);
void rndr_shift_sprite (SDL_Surface *spr, unsigned char alpha, char hshift, char sshift, char lshift);

#endif // RNDR_H__

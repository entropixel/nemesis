/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

// Animation definitions for all sprite types
// This file will eventually get pretty nasty

#include <SDL.h>

#include "anim.h"
#include "obj.h"

animframe_t char_anim_frames [] =
{
	{ 0, -1, -1 }, // standing
	{ -1, -1, -1 }, // unused
	{ -1, -1, -1 },
	{ -1, -1, -1 },
	// Walking animations
	{ 0, 4, char_anim_walk2 },
	{ 4, 4, char_anim_walk3 },
	{ 5, 4, char_anim_walk4 },
	{ 4, 4, char_anim_walk5 },
	{ 0, 4, char_anim_walk6 },
	{ 6, 4, char_anim_walk7 },
	{ 7, 4, char_anim_walk8 },
	{ 6, 4, char_anim_walk1 }
};

unsigned short char_anim_rots [] = { 0, 1, 2, 3, 4, 5, 6, 7 };

anim_t char_anim =
{
	32,
	32,
	char_anim_frames,
	char_anim_rots
};

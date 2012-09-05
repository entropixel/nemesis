/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

// Animation definitions for all sprite types
// This file will eventually get pretty nasty

#include <SDL.h>

#include "int.h"
#include "anim.h"
#include "obj.h"

uint16 generic_anim_rots [] = { 0, 1, 2, 3, 4, 5, 6, 7 };

animframe_t char_anim_frames [] =
{
	{ 0, 360, char_anim_idle2 }, // standing
	{ 1, 30, char_anim_idle1 },
	{ 0, 60, char_anim_tired2 },
	{ 2, 60, char_anim_tired1 },
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

anim_t char_anim =
{
	32,
	32,
	char_anim_frames,
	generic_anim_rots
};

animframe_t slime_anim_frames [] =
{
	{ 0, 20, 1 },
	{ 6, 20, 2 },
	{ 7, 20, 3 },
	{ 6, 20, 0 }
};

anim_t slime_anim =
{
	16,
	16,
	slime_anim_frames,
	generic_anim_rots
};

animframe_t torch_anim_frames [] =
{
	{ 0, 12, 1 },
	{ 1, 12, 2 },
	{ 2, 12, 0 }
};

anim_t torch_anim =
{
	16,
	16,
	torch_anim_frames,
	generic_anim_rots
};

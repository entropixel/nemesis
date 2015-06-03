/* See LICENSE file for copyright and licensing details */
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
	{ 6, 4, char_anim_walk1 },
	{ 8, 6, char_anim_punch2 },
	{ 9, 6, char_anim_punch3 },
	{ 10, 6, char_anim_idle1 }
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
	{ 0, 15, slime_anim_crawl2 },
	{ 4, 15, slime_anim_crawl3 },
	{ 5, 15, slime_anim_crawl4 },
	{ 4, 15, slime_anim_crawl1 },
	{ 6, 15, slime_anim_attack2 },
	{ 7, 15, slime_anim_attack3 },
	{ 6, 15, slime_anim_attack4 },
	{ 0, 15, slime_anim_crawl1 }
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

/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef ANIM_H__
#define ANIM_H__

enum
{
	char_anim_idle1,
	char_anim_idle2,
	char_anim_tired1,
	char_anim_tired2,
	char_anim_walk1,
	char_anim_walk2,
	char_anim_walk3,
	char_anim_walk4,
	char_anim_walk5,
	char_anim_walk6,
	char_anim_walk7,
	char_anim_walk8,
};

enum
{
	ROT_DOWN,
	ROT_DOWNRIGHT,
	ROT_RIGHT,
	ROT_UPRIGHT,
	ROT_UP,
	ROT_UPLEFT,
	ROT_LEFT,
	ROT_DOWNLEFT
};

// animation frame
typedef struct
{
	uint16 pos;
	uint16 duration;
	uint8 next;
} animframe_t;

// animation definition
typedef struct
{
	uint16 w;
	uint16 h;
	animframe_t *frames;
	uint16 *rots;
} anim_t;

extern anim_t char_anim;
extern anim_t slime_anim;
extern anim_t torch_anim;

#endif // ANIM_H__

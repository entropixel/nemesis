/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef ANIM_H__
#define ANIM_H__

enum
{
	char_anim_stand,
	char_anim_un1,
	char_anim_un2,
	char_anim_un3,
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
	unsigned short pos;
	char duration;
	unsigned char next;
} animframe_t;

// animation definition
typedef struct
{
	unsigned short w;
	unsigned short h;
	animframe_t *frames;
	unsigned short *rots;
} anim_t;

extern anim_t char_anim;

#endif // ANIM_H__

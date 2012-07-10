/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef OBJ_H__
#define OBJ_H__

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

// generic map object
typedef struct obj_s
{
	SDL_Texture *tex;
	SDL_Rect show; // Portion of tex that we show
	SDL_Rect dest; // Where we draw that portion of tex

	// animation stuff
	anim_t *anim;
	unsigned char frame; // Points to index in anim->frames
	unsigned char rot; // Points to index in anim->rots
	char frametics; // How many more tics to stay on current frame

	float x;
	float y;
	void *thinker; // TODO
	struct obj_s *next; // TODO
} obj_t;

obj_t *obj_create (float x, float y, SDL_Texture *tex, anim_t *anim, unsigned short frame, unsigned char rot);
void obj_set_frame (obj_t *obj, unsigned short frame);
void obj_set_rot (obj_t *obj, unsigned char rot);
void obj_adv_frame (obj_t *obj);

#endif // OBJ_H__

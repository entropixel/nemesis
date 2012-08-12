/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef OBJ_H__
#define OBJ_H__

// generic map object
typedef struct obj_s
{
	SDL_Texture *tex;
	SDL_Rect show; // Portion of tex that we show
	SDL_Rect dest; // Where we draw that portion of tex

	// animation stuff
	anim_t *anim;
	uint8 frame; // Point32 to index in anim->frames
	uint8 rot; // Point32 to index in anim->rots
	uint8 frametics; // How many more tics to stay on current frame

	float x;
	float y;
	SDL_Rect hitbox;
	void *thinker; // TODO
	struct obj_s *next;
} obj_t;

obj_t *obj_create (float x, float y, SDL_Texture *tex, anim_t *anim, uint16 frame, uint8 rot);
void obj_set_frame (obj_t *obj, uint16 frame);
void obj_set_rot (obj_t *obj, uint8 rot);
void obj_adv_frame (obj_t *obj);

extern obj_t *obj_list_head, *obj_list_tail;

#endif // OBJ_H__

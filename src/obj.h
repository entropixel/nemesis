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
	uint8 frame; // Point to index in anim->frames
	uint8 rot; // Point to index in anim->rots
	uint16 frametics; // How many more tics to stay on current frame

	float x;
	float y;
	uint8 hitb_x;
	uint8 hitb_y;
	SDL_Rect hitbox;
	void (*thinker) (struct obj_s *obj);
	struct obj_s *next;
} obj_t;

// object thinker
typedef void (*objthink_f) (obj_t *obj);

struct tile_t;

obj_t *obj_create (float x, float y, SDL_Texture *tex, anim_t *anim, uint16 frame, uint8 rot, objthink_f thinker);
void obj_set_hitbox (obj_t *obj, uint8 x, uint8 y, uint16 w, uint16 h);
void obj_set_frame (obj_t *obj, uint16 frame);
void obj_set_rot (obj_t *obj, uint8 rot);
void obj_adv_frame (obj_t *obj);
void obj_do_advframes (void);
void obj_collide_tiles (obj_t *obj, struct tile_t *tiles, uint16 width);
void obj_do_thinkers (void);

extern obj_t *obj_list_head, *obj_list_tail;

#endif // OBJ_H__

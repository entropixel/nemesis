/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef AI_H__
#define AI_H__

typedef enum
{
	N_UNSET,
	N_OPENED,
	N_CLOSED,
	N_BLOCKED
} nodetype_e;

typedef struct mapnode_s
{
	nodetype_e type;
	uint8 x;
	uint8 y;
	uint32 g;
	uint32 h;
	struct mapnode_s *parent;
} mapnode_t;

typedef struct
{
	obj_t *target;
	fixed targx;
	fixed targy;
	mapnode_t *nodelist;
	uint16 nodeidx;
	enum
	{
		ai_null,
		ai_wander,
		ai_chase,
		ai_attack
	} state;
} aidata_t;

mapnode_t *ai_get_path (obj_t *obj, obj_t *targ, level_t *l);
void ai_thinker (obj_t *obj);

#endif // AI_H__

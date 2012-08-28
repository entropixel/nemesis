/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef TILE_H__
#define TILE_H__

typedef struct
{
	uint16 x;
	uint16 y;
} tileoffs_t;

typedef struct tile_t
{
	SDL_Texture *sheet;
	tileoffs_t offs;
	uint16 flags;
} tile_t;

extern tileoffs_t dungeon_tileoffs [];
extern int32 dungeon_tileoffs_size;
enum
{
	dun_floor = 0,
	dun_wall_n = 1,
	dun_wall_e = 4,
	dun_wall_s = 7,
	dun_wall_w = 10,
	dun_wall_ine = 13,
	dun_wall_inw = 16,
	dun_wall_ise = 19,
	dun_wall_isw = 22,
	dun_wall_one,
	dun_wall_onw,
	dun_wall_ose,
	dun_wall_osw
};

enum
{
	TF_SOLID = 1 << 0
};

#endif // TILE_H__

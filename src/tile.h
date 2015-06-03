/* See LICENSE file for copyright and licensing details */
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
	uint8 level;
	struct
	{
		fixed x;
		fixed y;
		fixed w;
		fixed h;
		fixed offsx;
		fixed offsy;
	} hitbox;
	uint16 flags;
} tile_t;

extern tileoffs_t dungeon_tileoffs [];
extern int32 dungeon_tileoffs_size;
enum
{
	dun_floor = 0,
	dun_wall_n = 2,
	dun_wall_e = 8,
	dun_wall_s = 14,
	dun_wall_w = 20,
	dun_wall_ine = 26,
	dun_wall_inw = 32,
	dun_wall_ise = 38,
	dun_wall_isw = 44,
	dun_wall_one = 50,
	dun_wall_onw = 56,
	dun_wall_ose = 62,
	dun_wall_osw = 68,
	dun_stair_n = 74,
	dun_stair_e = 78,
	dun_stair_s = 82,
	dun_stair_w = 86,
	dun_black = 90,
};

enum
{
	TF_SOLID = 1 << 0,
	TF_STAIR = 1 << 1
};

#endif // TILE_H__

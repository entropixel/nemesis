/* See LICENSE file for copyright and licensing details */
#ifndef LEVEL_H__
#define LEVEL_H__

struct tile_t;

typedef struct level_t
{
	char path [32];

	uint8 w;
	uint8 h;

	struct tile_t *tiles;
	uint32 *offs;
} level_t;

level_t *level_new (uint8 w, uint8 h, const char *path);
void level_save (level_t *l);
level_t *level_load (const char *path);
void level_edit_tile (int8 offs);

#endif // LEVEL_H__

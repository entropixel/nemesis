/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <SDL.h>

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "level.h"
#include "rndr.h"
#include "tile.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

level_t *level_new (uint8 w, uint8 h, const char *path)
{
	level_t *ret = malloc (sizeof (level_t));
	time_t t = time (NULL);
	struct tm *now = localtime (&t);

	if (!ret)
		return NULL;

	if (!path)
		strftime (ret->path, 32, "levels/%m%d%y-%H%M.nrf", now);
	else
		strncpy (ret->path, path, 32);

	ret->w = w;
	ret->h = h;

	ret->tiles = malloc (w * h * sizeof (struct tile_t));
	ret->offs = malloc (w * h * sizeof (int32));

	if (!ret->tiles || !ret->offs)
	{
		free (ret->tiles);
		free (ret->offs);
		free (ret);
		return NULL;
	}

	memset (ret->tiles, 0, sizeof (struct tile_t) * w * h);
	memset (ret->offs, 0, sizeof (int32) * w * h);

	printf ("saving level data to %s\n", ret->path);

	return ret;
}

// Terrible level format at the moment, will need to be expanded upon when more stuff gets done in the rest of the engine
extern light_t *light_list_head;
extern obj_t *obj_list_head;
void level_save (level_t *l)
{
	int32 fd = open (l->path, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IRUSR | S_IWUSR);
	off_t pos;
	uint32 i, j;
	light_t *light_it = light_list_head;
	obj_t *obj_it = obj_list_head;

	if (fd < 0)
		return;

	if (write (fd, &(l->w), 1) != 1 || write (fd, &(l->w), 1) != 1)
		goto level_save_error;

	for (i = 0; i < l->w; i++)
		for (j = 0; j < l->h; j++)
			if (write (fd, &(l->tiles [i * l->w + j].flags), 2) != 2 || write (fd, &(l->offs [i * l->w + j]), 4) != 4)
				goto level_save_error;

	i = 0;
	pos = lseek (fd, 0, SEEK_CUR);
	write (fd, &i, 4);
	while (light_it)
	{
		int32 total = 0;
		total += write (fd, &(light_it->x), 1);
		total += write (fd, &(light_it->y), 1);
		total += write (fd, &(light_it->hue), 1);
		total += write (fd, &(light_it->sat), 1);
		total += write (fd, &(light_it->bright), 1);
		total += write (fd, &(light_it->falloff), 1);
		total += write (fd, &(light_it->flicker), 1);

		printf ("saving light: %i\n", total);
		if (total != 7)
			goto level_save_error;

		light_it = light_it->next;
		i ++;
	}

	printf ("Completed light loop\n");

	lseek (fd, pos, SEEK_SET);
	write (fd, &i, 4);
	lseek (fd, 0, SEEK_END);

	while (obj_it)
	{
		uint32 x = (uint32)obj_it->x, y = (uint32)obj_it->y;

		if (obj_it->anim == &torch_anim)
			if (write (fd, &(obj_it->rot), 1) != 1 || write (fd, &x, 4) != 4 || write (fd, &y, 4) != 4)
				goto level_save_error;

		obj_it = obj_it->next;
	}

	close (fd);
	return;

	level_save_error:
	close (fd);
	return;
}

void update_tiles (void);
extern SDL_Texture *torchtex;
level_t *level_load (const char *path)
{
	int32 fd = open (path, O_RDONLY | O_BINARY);
	uint8 w, h;
	uint32 i, j, numlights;
	level_t *ret = NULL;

	if (fd < 0)
		return NULL;

	if (read (fd, &w, 1) != 1 || read (fd, &h, 1) != 1)
		goto level_load_error;

	ret = level_new (w, h, path);

	for (i = 0; i < w; i++)
		for (j = 0; j < h; j++)
			if (read (fd, &(ret->tiles [i * w + j].flags), 2) != 2 || read (fd, &(ret->offs [i * w + j]), 4) != 4)
				goto level_load_error;

	if (read (fd, &numlights, 4) != 4)
		goto level_load_error;

	printf ("read %i lights\n", numlights);

	for (i = 0; i < numlights; i++)
	{
		uint8 x, y, hue, sat, bright, falloff, flicker;
		int32 total = 0;

		total += read (fd, &x, 1);
		total += read (fd, &y, 1);
		total += read (fd, &hue, 1);
		total += read (fd, &sat, 1);
		total += read (fd, &bright, 1);
		total += read (fd, &falloff, 1);
		total += read (fd, &flicker, 1);

		if (total != 7)
			goto level_load_error;

		printf ("adding light %i %i %i %i %i %i %i\n", x, y, hue, sat, bright, falloff, flicker);
		rndr_add_light (x, y, hue, sat, bright, falloff, flicker);
	}

	for (i = 0; i < numlights - 1; i++)
	{
		uint8 rot;
		uint32 x, y;

		if (read (fd, &rot, 1) != 1 || read (fd, &x, 4) != 4 || read (fd, &y, 4) != 4)
			goto level_load_error;

		obj_create (x, y, torchtex, &torch_anim, 0, rot, NULL);
	}

	close (fd);
	return ret;

	level_load_error:
	close (fd);
	if (ret)
	{
		free (ret->tiles);
		free (ret->offs);
		free (ret);
	}

	return NULL;
}


extern level_t *level; // FIXME loool
extern obj_t *player;
void level_edit_tile (int8 offs)
{
	static uint32 lastused = 0;
	uint32 *level_offs = &(level->offs [((player->hitbox.x + player->hitbox.w / 2) / 16) * level->w + ((player->hitbox.y + player->hitbox.h / 2) / 16)]);

	if (!(*level_offs))
		*level_offs = dungeon_tileoffs_size;

	if (offs)
	{
		*level_offs += offs;
		*level_offs %= dungeon_tileoffs_size;
		lastused = *level_offs;
	}
	else
	{
		*level_offs = lastused;
	}

	update_tiles ();

	return;
}

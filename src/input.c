/* See LICENSE file for copyright and licensing details */
#include <SDL.h>

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "rndr.h"
#include "level.h"
#include "tile.h"
#include "input.h"

uint32 keymask = 0;

extern int8 renderlights, renderdbg, running, editmode;
extern obj_t *player, *obj_list_head;
extern light_t *light_list_head;
extern level_t *level;
extern SDL_Texture *torchtex;

void input_get (SDL_Event *ev)
{
	while (SDL_PollEvent (ev))
	{
		switch (ev->type)
		{
			case SDL_KEYUP:
				switch (ev->key.keysym.sym)
				{
					case SDLK_UP:
						keymask &= ~(UPK);
						break;
					case SDLK_RIGHT:
						keymask &= ~(RIGHTK);
						break;
					case SDLK_DOWN:
						keymask &= ~(DOWNK);
						break;
					case SDLK_LEFT:
						keymask &= ~(LEFTK);
						break;
					case SDLK_F1:
						editmode = !editmode;
						player->flags ^= OF_NOCLIP;
						if (editmode)
							SDL_SetTextureAlphaMod (player->tex, 0);
						else
							SDL_SetTextureAlphaMod (player->tex, 255);
						break;
					case SDLK_F2:
						renderlights = !renderlights;
						break;
					case SDLK_F3:
						renderdbg = !renderdbg;
						break;
					case SDLK_a:
						if (editmode)
							level_edit_tile (-1);
						break;
					case SDLK_s:
						if (editmode)
							level_edit_tile (1);
						break;
					case SDLK_d:
						if (editmode)
							level_edit_tile (0);
						break;
					case SDLK_f:
						keymask &= ~(ATK);
						break;
					case SDLK_z:
					case SDLK_x:
						if (editmode)
						{
						uint8 *t = &level->tiles [(obj_centerx (player) >> FRAC * 2) * level->w + (obj_centery (player) >> FRAC * 2)].level;
							if (ev->key.keysym.sym == SDLK_z)
							{
								if (!(*t))
									*t = 2;
								else
									(*t) --;
							}
							else
							{
								(*t) ++;
								*t %= 3;
							}
						}
						break;
					case SDLK_t:
					case SDLK_y:
						if (editmode)
						{
							obj_create ((obj_centerx (player) >> FRAC * 2) << FRAC * 2,
							            (obj_centery (player) >> FRAC * 2) << FRAC * 2,
							            torchtex, &torch_anim, 0, player->rot, NULL, NULL);
							if (ev->key.keysym.sym == SDLK_t)
								rndr_add_light (obj_centerx (player) >> FRAC * 2, obj_centery (player) >> FRAC * 2,
												28, 100, 142, 24, 10);
							else
								rndr_add_light (obj_centerx (player) >> FRAC * 2, obj_centery (player) >> FRAC * 2,
												28, 100, 107, 24, 10);
						}
						break;
					case SDLK_c:
						if (editmode)
						{
							obj_t *it = obj_list_head, *next;
							rndr_clear_lights ();
							rndr_add_light (0, 0, 0, 0, 48, 0, 0);
							while (it)
							{
								next = it->next;
								if (it->anim == &torch_anim)
									obj_destroy (it);
								it = next;
							}
						}
						break;
					case SDLK_w:
						if (editmode)
							level_save (level);
						break;
				}
			break;
			case SDL_KEYDOWN:
				switch (ev->key.keysym.sym)
				{
					case SDLK_UP:
						keymask |= UPK;
						break;
					case SDLK_RIGHT:
						keymask |= RIGHTK;
						break;
					case SDLK_DOWN:
						keymask |= DOWNK;
						break;
					case SDLK_LEFT:
						keymask |= LEFTK;
						break;
					case SDLK_f:
						keymask |= ATK;
				}
			break;
			case SDL_QUIT:
				running = 0;
			break;
		}
	}

	return;
}

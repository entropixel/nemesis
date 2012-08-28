/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <SDL.h>

#include "int.h"
#include "anim.h"
#include "obj.h"
#include "level.h"
#include "input.h"

uint32 keymask = 0;

extern int8 renderlights, renderdbg, running, editmode;
extern obj_t *player;

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
				}
			break;
			case SDL_QUIT:
				running = 0;
			break;
		}
	}

	return;
}

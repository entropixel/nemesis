/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <SDL.h>

#include "int.h"
#include "input.h"

uint32 keymask = 0;

extern int8 renderlights, renderdbg, running;

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
					case SDLK_F2:
						renderlights = !renderlights;
						break;
					case SDLK_F3:
						renderdbg = !renderdbg;
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

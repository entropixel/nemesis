/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>

#if !SDL_VERSION_ATLEAST(2,0,0)
#error "Need SDL 2.0"
#endif

#include "anim.h"
#include "obj.h"

SDL_Window *win = NULL;
SDL_Renderer *rndr = NULL;

enum
{
	UPK = 1,
	RIGHTK = 2,
	DOWNK = 4,
	LEFTK = 8
};

unsigned int keymask = 0;

int main (int argc, char **argv)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "SDL_Init failed (%s)\n", SDL_GetError ());
		return 1;
	}

	if (!(IMG_Init (IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		fprintf (stderr, "IMG_Init failed (%s)\n", IMG_GetError ());
		return 1;
	}

	if (!(win = SDL_CreateWindow ("nemesis (" __DATE__ ")", SDL_WINDOWPOS_CENTERED,
	                              SDL_WINDOWPOS_CENTERED, 256, 160, 0)))
	{
		fprintf (stderr, "SDL_CreateWindow failed (%s)\n", SDL_GetError ());
		return 1;
	}

	if (!(rndr = SDL_CreateRenderer (win, -1, SDL_RENDERER_ACCELERATED)))
	{
		fprintf (stderr, "SDL_CreateRenderer failed (%s)\n", SDL_GetError ());
		return 1;
	}

	// testing crap now :D
	SDL_Surface *plsprite = IMG_Load ("img/objects/player/male_unarm/nosleeve_shorts_short.png");
	obj_t *player = obj_create (32, 32, SDL_CreateTextureFromSurface (rndr, plsprite), &char_anim, char_anim_stand, ROT_DOWNRIGHT);

	SDL_Event ev;

	char running = 1;
	while (running)
	{
		int preticks = SDL_GetTicks (), postticks;

		while (SDL_PollEvent (&ev))
		{
			switch (ev.type)
			{
				case SDL_KEYUP:
					switch (ev.key.keysym.sym)
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
					}
				break;
				case SDL_KEYDOWN:
					switch (ev.key.keysym.sym)
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

		if (keymask)
		{
			switch (keymask)
			{
				case UPK:
					obj_set_rot (player, ROT_UP);
					player->y -= 1.41f;
				break;
				case UPK | RIGHTK:
					obj_set_rot (player, ROT_UPRIGHT);
					player->x += 1.0f;
					player->y -= 1.0f;
				break;
				case RIGHTK:
					obj_set_rot (player, ROT_RIGHT);
					player->x += 1.41f;
				break;
				case RIGHTK | DOWNK:
					obj_set_rot (player, ROT_DOWNRIGHT);
					player->x += 1.0f;
					player->y += 1.0f;
				break;
				case DOWNK:
					obj_set_rot (player, ROT_DOWN);
					player->y += 1.41f;
				break;
				case DOWNK | LEFTK:
					obj_set_rot (player, ROT_DOWNLEFT);
					player->x -= 1.0f;
					player->y += 1.0f;
				break;
				case LEFTK:
					obj_set_rot (player, ROT_LEFT);
					player->x -= 1.41f;
				break;
				case LEFTK | UPK:
					obj_set_rot (player, ROT_UPLEFT);
					player->x -= 1.0f;
					player->y -= 1.0f;
				break;
				default:
				break;
			}

			if (player->frame == char_anim_stand)
				obj_set_frame (player, char_anim_walk1);

			player->dest.x = (short) player->x;
			player->dest.y = (short) player->y;
		}
		else
			obj_set_frame (player, char_anim_stand);

		SDL_RenderClear (rndr);
		SDL_RenderCopy (rndr, player->tex, &(player->show), &(player->dest));
		SDL_RenderPresent (rndr);

		obj_adv_frame (player);

		postticks = SDL_GetTicks ();
		if (postticks - preticks < (1000/60))
			SDL_Delay ((1000/60) - (postticks - preticks));
	}

	SDL_DestroyRenderer (rndr);
	SDL_DestroyWindow (win);
	IMG_Quit ();
	SDL_Quit ();
	return 0;
}

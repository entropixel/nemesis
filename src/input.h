/* See LICENSE file for copyright and licensing details */
#ifndef INPUT_H__
#define INPUT_H__

void input_get (SDL_Event *ev);

enum
{
	UPK = 1,
	RIGHTK = 2,
	DOWNK = 4,
	LEFTK = 8,
	ATK = 16
};

extern uint32 keymask;

#endif // INPUT_H__

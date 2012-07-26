/* nemesis engine
   [c] 2012 Kyle Davis, All Rights Reserved
   Unauthorized redistribution is prohibited. */

#ifndef INPUT_H__
#define INPUT_H__

void input_get (SDL_Event *ev);

enum
{
	UPK = 1,
	RIGHTK = 2,
	DOWNK = 4,
	LEFTK = 8
};

extern unsigned int keymask;

#endif // INPUT_H__

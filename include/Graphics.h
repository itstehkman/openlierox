/////////////////////////////////////////
//
//             OpenLieroX
//
// code under LGPL, based on JasonBs work,
// enhanced by Dark Charlie and Albert Zeyer
//
//
/////////////////////////////////////////


// Graphics header file
// Created 30/6/02
// Jason Boettcher


#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__


// Gui graphics
typedef struct {
	SDL_Surface		*bmpMouse[4];
	SDL_Surface		*bmpScrollbar;
	SDL_Surface		*bmpSliderBut;
} gfxgui_t;


// Game graphics
typedef struct {
	SDL_Surface		*bmpCrosshair;
	SDL_Surface		*bmpMuzzle;
	SDL_Surface		*bmpExplosion;
	SDL_Surface		*bmpSmoke;
	SDL_Surface		*bmpChemSmoke;
	SDL_Surface		*bmpSpawn;
	SDL_Surface		*bmpHook;
	SDL_Surface		*bmpGameover;
	SDL_Surface		*bmpInGame;
	SDL_Surface		*bmpScoreboard;
    SDL_Surface		*bmpViewportMgr;
	SDL_Surface		*bmpSparkle;
	SDL_Surface		*bmpInfinite;
	SDL_Surface		*bmpLag;

	SDL_Surface		*bmpBonus;
	SDL_Surface		*bmpHealth;
} gfxgame_t;


extern	gfxgui_t	gfxGUI;
extern	gfxgame_t	gfxGame;


// Routines
int		LoadGraphics(void);
void	ShutdownGraphics(void);



#endif  //  __GRAPHICS_H__

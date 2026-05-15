#ifndef SDL_COMPAT_H
#define SDL_COMPAT_H

#define SDL_ENABLE_OLD_NAMES
#include <SDL3/SDL.h>

extern SDL_Window *sdlWindow;
extern SDL_GLContext sdlGLContext;
extern SDL_Palette *sdlGamePalette;

SDL_Surface *SDL_CreateIndexedSurface(int width, int height);
void SDL_SetGamePalette(SDL_Surface *surface, const SDL_Color *colors, int firstcolor, int ncolors);
void SDL_SetGamePaletteColor(SDL_Surface *surface, const SDL_Color *color, int firstcolor);
int SDL_NumJoysticksCompat(void);
SDL_Joystick *SDL_OpenJoystickByIndex(int index);

#endif

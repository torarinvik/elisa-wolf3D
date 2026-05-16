#ifndef ELISA_WOLF3D_SDL_H
#define ELISA_WOLF3D_SDL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t wolf3d_sdl_set_palette_colors(void *palette, const uint8_t *colors, int32_t firstcolor, int32_t ncolors);
int32_t wolf3d_sdl_set_surface_palette(void *surface, void *palette);
int32_t wolf3d_sdl_set_game_palette(void *surface, void *palette, const uint8_t *colors, int32_t firstcolor, int32_t ncolors);

#ifdef __cplusplus
}
#endif

#endif

#ifndef ELISA_WOLF3D_PALETTE_H
#define ELISA_WOLF3D_PALETTE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t wolf3d_convert_palette_6bit_rgb_to_8bit_rgb(uint8_t *src, uint8_t *dst, int32_t count);

#ifdef __cplusplus
}
#endif

#endif

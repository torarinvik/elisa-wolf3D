#ifndef ELISA_WOLF3D_VIDEO_H
#define ELISA_WOLF3D_VIDEO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t wolf3d_log2_ceil(uint32_t value);
uint32_t wolf3d_fizzle_mask_for_bits(int32_t bits);
int32_t wolf3d_validate_munge_pic_width(uint32_t width);
int32_t wolf3d_munge_pic_from_temp(uint8_t *source, uint8_t *temp, uint32_t width, uint32_t height);
int32_t wolf3d_validate_fizzle_frames(uint32_t frames);
int32_t wolf3d_fizzle_bits_for_dimensions(uint32_t width, uint32_t height);
uint32_t wolf3d_fizzle_mask_for_dimensions(uint32_t width, uint32_t height);

#ifdef __cplusplus
}
#endif

#endif

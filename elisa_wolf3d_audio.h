#ifndef ELISA_WOLF3D_AUDIO_H
#define ELISA_WOLF3D_AUDIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t wolf3d_playback_pan_for_position(int32_t pos);
int32_t wolf3d_validate_playback_position(int32_t leftpos, int32_t rightpos);
int16_t wolf3d_interpolate_u8_sample_to_s16(float csample, uint8_t *samples, int32_t size);

#ifdef __cplusplus
}
#endif

#endif

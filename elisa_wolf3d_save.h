#ifndef ELISA_WOLF3D_SAVE_H
#define ELISA_WOLF3D_SAVE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t wolf3d_checksum(uint8_t *source, uint32_t size, int32_t seed);

#ifdef __cplusplus
}
#endif

#endif

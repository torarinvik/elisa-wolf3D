#ifndef ELISA_WOLF3D_PAGEFILE_H
#define ELISA_WOLF3D_PAGEFILE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t wolf3d_validate_page_offset(uint32_t page_offset, uint32_t data_start, uint32_t file_size);

#ifdef __cplusplus
}
#endif

#endif

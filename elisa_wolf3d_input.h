#ifndef ELISA_WOLF3D_INPUT_H
#define ELISA_WOLF3D_INPUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t wolf3d_clamp_joy_axis(int32_t value);
int32_t wolf3d_apply_joy_hat_axis(int32_t value, int32_t negative_pressed, int32_t positive_pressed);
int32_t wolf3d_translate_mouse_buttons(int32_t buttons, int32_t middle_pressed, int32_t right_pressed);

#ifdef __cplusplus
}
#endif

#endif

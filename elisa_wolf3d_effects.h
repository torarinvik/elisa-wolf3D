#ifndef ELISA_WOLF3D_EFFECTS_H
#define ELISA_WOLF3D_EFFECTS_H

#ifdef __cplusplus
extern "C" {
#endif

int wolf3d_effect_audio_device(void);
int wolf3d_effect_audio_load(void);
int wolf3d_effect_audio_playback(void);
int wolf3d_effect_video_window(void);
int wolf3d_effect_video_surface(void);
int wolf3d_effect_video_palette(void);
int wolf3d_effect_video_render(void);
int wolf3d_effect_input_poll(void);
int wolf3d_effect_input_grab(void);
int wolf3d_effect_filesystem_read(void);
int wolf3d_effect_filesystem_write(void);
int wolf3d_effect_time_read(void);
int wolf3d_effect_time_delay(void);
int wolf3d_effect_diagnostics_console(void);
int wolf3d_effect_process_abort(void);

#ifdef __cplusplus
}
#endif

#endif

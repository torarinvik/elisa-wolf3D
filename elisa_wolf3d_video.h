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
int32_t wolf3d_align_picture_x(int32_t x);
int32_t wolf3d_pic_index_for_chunk(int32_t chunknum, int32_t startpics);
int32_t wolf3d_latchpic_index(int32_t picnum, int32_t latchpics_lump_start);
int32_t wolf3d_latch_tile_surface_height(int32_t num_tiles);
int32_t wolf3d_latch_tile_dest_x(int32_t tile_index);
int32_t wolf3d_latch_tile_dest_y(int32_t tile_index);
int32_t wolf3d_legacy_vl_set_vga_plane_mode(void);
int32_t wolf3d_legacy_vl_set_palette(SDL_Color *palette, int32_t forceupdate);
int32_t wolf3d_legacy_vl_get_palette(SDL_Color *palette);
int32_t wolf3d_legacy_vl_fill_palette(int32_t red, int32_t green, int32_t blue);
int32_t wolf3d_latch_draw_pic_on_screen(int32_t picnum, int32_t x, int32_t y);
int32_t wolf3d_latch_draw_pic_scaled_coord_on_screen(int32_t picnum, int32_t scx, int32_t scy);
int32_t wolf3d_fizzle_fade_on_locked_buffers(uint8_t *source_pixels, int32_t source_pitch, uint8_t *screen_copy_pixels, int32_t screen_copy_pitch, int32_t x1, int32_t y1, int32_t width, int32_t height, uint32_t frames, int32_t abortable, int32_t usedoublebuffering, int32_t rndbits_y, uint32_t rndmask, int32_t source_width, int32_t source_height, int32_t screen_width, int32_t screen_height, void *screen_copy, void *screen_buffer, void *screen);
int32_t wolf3d_legacy_in_start_ack(void);
int32_t wolf3d_legacy_in_check_ack(void);
int32_t wolf3d_video_install_runtime_state(int32_t screen_width, int32_t screen_height, int32_t screen_bits, int32_t usedoublebuffering, int32_t screen_pitch, int32_t buffer_pitch, int32_t cur_pitch, int32_t scale_factor, void *screen_surface, void *screen_buffer_surface, void *cur_surface, void *game_palette);
int32_t wolf3d_video_set_screen_width(int32_t screen_width);
int32_t wolf3d_video_set_screen_height(int32_t screen_height);
int32_t wolf3d_video_set_screen_bits(int32_t screen_bits);
int32_t wolf3d_video_set_usedoublebuffering(int32_t usedoublebuffering);
int32_t wolf3d_video_get_screen_width(void);
int32_t wolf3d_video_get_screen_height(void);
int32_t wolf3d_video_get_screen_bits(void);
int32_t wolf3d_video_get_screen_pitch(void);
int32_t wolf3d_video_get_buffer_pitch(void);
int32_t wolf3d_video_get_cur_pitch(void);
int32_t wolf3d_video_get_scale_factor(void);
int32_t wolf3d_video_get_usedoublebuffering(void);
void *wolf3d_video_get_screen_surface(void);
void *wolf3d_video_get_screen_buffer_surface(void);
void *wolf3d_video_get_cur_surface(void);
void *wolf3d_video_get_game_palette(void);
int32_t wolf3d_vl_fade_out_from_host(int32_t start, int32_t end, int32_t red, int32_t green, int32_t blue, int32_t steps);
int32_t wolf3d_vl_fade_in_from_host(int32_t start, int32_t end, uint8_t *palette, int32_t steps);
int32_t wolf3d_legacy_get_usedoublebuffering(void);
int32_t wolf3d_legacy_get_screen_bits(void);
int32_t wolf3d_legacy_get_screen_width(void);
int32_t wolf3d_legacy_get_screen_height(void);
int32_t wolf3d_legacy_get_screen_pitch(void);
int32_t wolf3d_legacy_get_buffer_pitch(void);
int32_t wolf3d_legacy_get_cur_pitch(void);
void *wolf3d_legacy_get_screen_surface(void);
void *wolf3d_legacy_get_screen_buffer_surface(void);
void *wolf3d_legacy_get_cur_surface(void);
void *wolf3d_legacy_get_game_palette(void);
uint32_t wolf3d_fizzle_pixels_per_frame(uint32_t width, uint32_t height, uint32_t frames);
int32_t wolf3d_configure_fizzle_state(uint32_t width, uint32_t height, uint32_t *rndbits_y, uint32_t *rndmask);
int32_t wolf3d_next_fizzle_first(int32_t usedoublebuffering, int32_t current_first);
int32_t wolf3d_draw_block_for_plot(int32_t scale);
int32_t wolf3d_copy_planar_to_linear(const uint8_t *source, uint8_t *dest, int32_t width, int32_t height, int32_t dest_pitch);
int32_t wolf3d_draw_planar_scaled_to_linear(const uint8_t *source, uint8_t *dest, int32_t width, int32_t height, int32_t dest_x, int32_t dest_y, int32_t scale, int32_t dest_pitch);
int32_t wolf3d_draw_planar_region_scaled_to_linear(const uint8_t *source, uint8_t *dest, int32_t orig_width, int32_t orig_height, int32_t src_x, int32_t src_y, int32_t dest_x, int32_t dest_y, int32_t width, int32_t height, int32_t scale, int32_t dest_pitch);
int32_t wolf3d_copy_linear_scaled_to_linear(const uint8_t *source, uint8_t *dest, int32_t src_x, int32_t src_y, int32_t width, int32_t height, int32_t dest_x, int32_t dest_y, int32_t scale, int32_t source_pitch, int32_t dest_pitch);
int32_t wolf3d_fill_linear_span(uint8_t *dest, int32_t width, int32_t color);
int32_t wolf3d_fill_linear_vline(uint8_t *dest, int32_t height, int32_t pitch, int32_t color);
int32_t wolf3d_fill_linear_rect(uint8_t *dest, int32_t width, int32_t height, int32_t pitch, int32_t color);
int32_t wolf3d_draw_pixel_on_linear_buffer(uint8_t *dest, int32_t x, int32_t y, int32_t color, int32_t pitch, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_get_pixel_from_linear_buffer(const uint8_t *source, int32_t x, int32_t y, int32_t pitch, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_hline_on_linear_buffer(uint8_t *dest, int32_t x, int32_t y, int32_t width, int32_t color, int32_t pitch, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_vline_on_linear_buffer(uint8_t *dest, int32_t x, int32_t y, int32_t height, int32_t color, int32_t pitch, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_bar_scaled_coord_on_linear_buffer(uint8_t *dest, int32_t scx, int32_t scy, int32_t scwidth, int32_t scheight, int32_t color, int32_t pitch, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_logical_plot_on_linear_buffer(uint8_t *dest, int32_t x, int32_t y, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_logical_hline_on_linear_buffer(uint8_t *dest, int32_t x1, int32_t x2, int32_t y, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_logical_vline_on_linear_buffer(uint8_t *dest, int32_t y1, int32_t y2, int32_t x, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_logical_bar_on_linear_buffer(uint8_t *dest, int32_t x, int32_t y, int32_t width, int32_t height, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_planar_block_on_linear_buffer(const uint8_t *source, uint8_t *dest, int32_t width, int32_t height, int32_t dest_x, int32_t dest_y, int32_t scale, int32_t dest_pitch, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_planar_region_on_linear_buffer(const uint8_t *source, uint8_t *dest, int32_t orig_width, int32_t orig_height, int32_t src_x, int32_t src_y, int32_t dest_x, int32_t dest_y, int32_t width, int32_t height, int32_t scale, int32_t dest_pitch, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_draw_prop_string_scaled(const uint8_t *text, const uint8_t *font_base, const int16_t *font_locations, const uint8_t *font_width, uint8_t *dest, int32_t font_height, int32_t scale, int32_t dest_pitch, int32_t color);
uint32_t wolf3d_apply_fizzle_pixels(const uint8_t *source, uint8_t *dest, int32_t x1, int32_t y1, int32_t width, int32_t height, int32_t source_pitch, int32_t dest_pitch, int32_t rndbits_y, uint32_t rndmask, uint32_t pixperframe, uint32_t start_rndval);
int32_t wolf3d_vwb_bar_on_linear_buffer(uint8_t *dest, int32_t x, int32_t y, int32_t width, int32_t height, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_vwb_plot_on_linear_buffer(uint8_t *dest, int32_t x, int32_t y, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_vwb_hlin_on_linear_buffer(uint8_t *dest, int32_t x1, int32_t x2, int32_t y, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_vwb_vlin_on_linear_buffer(uint8_t *dest, int32_t y1, int32_t y2, int32_t x, int32_t color, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_vwb_draw_prop_string_on_linear_buffer(const uint8_t *text, const uint8_t *font_base, const int16_t *font_locations, const uint8_t *font_width, uint8_t *dest, int32_t font_height, int32_t scale, int32_t dest_pitch, int32_t color);
int32_t wolf3d_vwb_draw_pic_on_linear_buffer(const uint8_t *source, int32_t width, int32_t height, uint8_t *dest, int32_t x, int32_t y, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_vwb_draw_pic_scaled_coord_on_linear_buffer(const uint8_t *source, int32_t width, int32_t height, uint8_t *dest, int32_t x, int32_t y, int32_t pitch, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_vh_update_screen(void *screen_buffer, void *screen);
int32_t wolf3d_validate_fizzle_frames(uint32_t frames);
int32_t wolf3d_fizzle_bits_for_dimensions(uint32_t width, uint32_t height);
uint32_t wolf3d_fizzle_mask_for_dimensions(uint32_t width, uint32_t height);
int32_t wolf3d_validate_fade_steps(int32_t steps);
int32_t wolf3d_validate_palette_index(int32_t color);
int32_t wolf3d_validate_scaled_draw_bounds(int32_t dest_x, int32_t dest_y, int32_t width, int32_t height, int32_t scale, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_validate_hline_bounds(int32_t dest_x, int32_t dest_y, int32_t width, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_validate_vline_bounds(int32_t dest_x, int32_t dest_y, int32_t height, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_validate_bar_bounds(int32_t dest_x, int32_t dest_y, int32_t width, int32_t height, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_validate_mem_to_latch_bounds(int32_t dest_x, int32_t dest_y, int32_t width, int32_t height, int32_t surface_width, int32_t surface_height);
int32_t wolf3d_validate_pixel_bounds(int32_t dest_x, int32_t dest_y, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_validate_fizzle_work_area(int32_t dest_x, int32_t dest_y, int32_t width, int32_t height, int32_t source_width, int32_t source_height, int32_t screen_width, int32_t screen_height);
int32_t wolf3d_fade_scale_6bit_color_component(int32_t component);
int32_t wolf3d_fade_interpolated_channel(int32_t origin, int32_t target, int32_t frame, int32_t steps);
int32_t wolf3d_measure_prop_string_width(const uint8_t *text, const uint8_t *font_width);
int32_t wolf3d_measure_prop_string(const uint8_t *text, uint16_t *width, uint16_t *height, int32_t font_height, const uint8_t *font_width);
int32_t wolf3d_measure_prop_string_safe(const uint8_t *text, uint16_t *width, uint16_t *height, int32_t font_height, const uint8_t *font_width);
int32_t wolf3d_inclusive_span_length(int32_t start, int32_t end);
int32_t wolf3d_convert_palette_6bit_rgb_to_sdl_colors(const uint8_t *src, uint8_t *dst, int32_t count);
int32_t wolf3d_fill_sdl_palette_rgb(uint8_t *dst, int32_t red, int32_t green, int32_t blue, int32_t count);
int32_t wolf3d_build_fade_out_sdl_palette(const uint8_t *origin, uint8_t *dst, int32_t start, int32_t end, int32_t red, int32_t green, int32_t blue, int32_t frame, int32_t steps, int32_t count);
int32_t wolf3d_build_fade_in_sdl_palette(const uint8_t *origin, const uint8_t *target, uint8_t *dst, int32_t start, int32_t end, int32_t frame, int32_t steps, int32_t count);
int32_t wolf3d_build_palette_shift_table(const uint8_t *base_palette, uint8_t *dst, int32_t frames, int32_t steps, int32_t target_red, int32_t target_green, int32_t target_blue, int32_t count);

#ifdef __cplusplus
}
#endif

#endif

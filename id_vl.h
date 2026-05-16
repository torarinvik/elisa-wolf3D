// ID_VL.H

// wolf compatability

// Win32
//#ifndef ID_VL.H 
//#define ID_VL.H
#ifndef ID_VL_H 
#define ID_VL_H

#include "wl_def.h"
#include "elisa_wolf3d_video.h"

void Quit (const char *error,...);

//===========================================================================

#define CHARWIDTH       2
#define TILEWIDTH       4

//===========================================================================

extern  boolean  fullscreen, usedoublebuffering;
extern  unsigned screenWidth, screenHeight, screenBits, screenPitch, bufferPitch, curPitch;
extern  unsigned scaleFactor;

extern  boolean  screenfaded;
extern  unsigned bordercolor;

extern SDL_Color gamepal[256];

//===========================================================================

//
// VGA hardware routines
//

#define VL_WaitVBL(a) SDL_Delay((a)*8)

#ifdef ID_VL_IMPLEMENTATION
void VL_SetVGAPlaneMode (void);
void VL_ConvertPalette(byte *srcpal, SDL_Color *destpal, int numColors);
void VL_FillPalette (int red, int green, int blue);
void VL_SetColor    (int color, int red, int green, int blue);
void VL_GetColor    (int color, int *red, int *green, int *blue);
void VL_SetPalette  (SDL_Color *palette, bool forceupdate);
void VL_GetPalette  (SDL_Color *palette);
void VL_FadeOut     (int start, int end, int red, int green, int blue, int steps);
void VL_FadeIn      (int start, int end, SDL_Color *palette, int steps);
byte *VL_LockSurface(SDL_Surface *surface);
void VL_UnlockSurface(SDL_Surface *surface);
byte VL_GetPixel        (int x, int y);
void VL_Plot            (int x, int y, int color);
void VL_Hlin            (unsigned x, unsigned y, unsigned width, int color);
void VL_Vlin            (int x, int y, int height, int color);
void VL_BarScaledCoord  (int scx, int scy, int scwidth, int scheight, int color);
void VL_MungePic                (byte *source, unsigned width, unsigned height);
void VL_DrawPicBare             (int x, int y, byte *pic, int width, int height);
void VL_MemToLatch              (byte *source, int width, int height,
                                    SDL_Surface *destSurface, int x, int y);
void VL_ScreenToScreen          (SDL_Surface *source, SDL_Surface *dest);
void VL_MemToScreenScaledCoord  (byte *source, int width, int height, int scx, int scy);
void VL_MemToScreenScaledCoord  (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                    int destx, int desty, int width, int height);
void VL_MaskedToScreen (byte *source, int width, int height, int x, int y);
void VL_LatchToScreenScaledCoord (SDL_Surface *source, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest);

#else
#define screenWidth wolf3d_video_get_screen_width()
#define screenHeight wolf3d_video_get_screen_height()
#define screenBits wolf3d_video_get_screen_bits()
#define screenPitch wolf3d_video_get_screen_pitch()
#define bufferPitch wolf3d_video_get_buffer_pitch()
#define curPitch wolf3d_video_get_cur_pitch()
#define scaleFactor wolf3d_video_get_scale_factor()
#define usedoublebuffering (wolf3d_video_get_usedoublebuffering() != 0)
#define screen ((SDL_Surface *) wolf3d_video_get_screen_surface())
#define screenBuffer ((SDL_Surface *) wolf3d_video_get_screen_buffer_surface())
#define curSurface ((SDL_Surface *) wolf3d_video_get_cur_surface())
#define sdlGamePalette ((SDL_Palette *) wolf3d_video_get_game_palette())

static inline void VL_SetVGAPlaneMode (void)
{
    (void) wolf3d_legacy_vl_set_vga_plane_mode();
}
void VL_SetTextMode (void);
void VL_Shutdown (void);

static inline void VL_ConvertPalette(byte *srcpal, SDL_Color *destpal, int numColors)
{
    (void) wolf3d_convert_palette_6bit_rgb_to_sdl_colors((const uint8_t *) srcpal, (uint8_t *) destpal, numColors);
}
static inline void VL_FillPalette (int red, int green, int blue)
{
    (void) wolf3d_legacy_vl_fill_palette(red, green, blue);
}
void VL_SetColor    (int color, int red, int green, int blue);
void VL_GetColor    (int color, int *red, int *green, int *blue);
static inline void VL_SetPalette  (SDL_Color *palette, bool forceupdate)
{
    (void) wolf3d_legacy_vl_set_palette(palette, forceupdate ? 1 : 0);
}
static inline void VL_GetPalette  (SDL_Color *palette)
{
    (void) wolf3d_legacy_vl_get_palette(palette);
}
static inline void VL_FadeOut     (int start, int end, int red, int green, int blue, int steps)
{
    (void) wolf3d_vl_fade_out_from_host(start, end, red, green, blue, steps);
}
static inline void VL_FadeIn      (int start, int end, SDL_Color *palette, int steps)
{
    (void) wolf3d_vl_fade_in_from_host(start, end, (uint8_t *) palette, steps);
}

static inline byte *VL_LockSurface(SDL_Surface *surface)
{
    if(surface == NULL)
        return NULL;
    if(SDL_LockSurface(surface) != 0)
        return NULL;
    return (byte *) surface->pixels;
}
static inline void VL_UnlockSurface(SDL_Surface *surface)
{
    if(surface != NULL)
        SDL_UnlockSurface(surface);
}

#define LOCK() VL_LockSurface(curSurface)
#define UNLOCK() VL_UnlockSurface(curSurface)

static inline byte VL_GetPixel        (int x, int y)
{
    return (byte) wolf3d_get_pixel_from_linear_buffer((const uint8_t *) curSurface->pixels, x, y, curPitch, screenWidth, screenHeight);
}
static inline void VL_Plot            (int x, int y, int color)
{
    (void) wolf3d_draw_pixel_on_linear_buffer((uint8_t *) curSurface->pixels, x, y, color, curPitch, screenWidth, screenHeight);
}
static inline void VL_Hlin            (unsigned x, unsigned y, unsigned width, int color)
{
    (void) wolf3d_draw_hline_on_linear_buffer((uint8_t *) curSurface->pixels, (int32_t) x, (int32_t) y, (int32_t) width, color, curPitch, screenWidth, screenHeight);
}
static inline void VL_Vlin            (int x, int y, int height, int color)
{
    (void) wolf3d_draw_vline_on_linear_buffer((uint8_t *) curSurface->pixels, x, y, height, color, curPitch, screenWidth, screenHeight);
}
static inline void VL_BarScaledCoord  (int scx, int scy, int scwidth, int scheight, int color)
{
    (void) wolf3d_draw_bar_scaled_coord_on_linear_buffer((uint8_t *) curSurface->pixels, scx, scy, scwidth, scheight, color, curPitch, screenWidth, screenHeight);
}
void inline VL_Bar      (int x, int y, int width, int height, int color)
{
    VL_BarScaledCoord(scaleFactor*x, scaleFactor*y,
        scaleFactor*width, scaleFactor*height, color);
}
void inline VL_ClearScreen(int color)
{
    SDL_FillRect(curSurface, NULL, color);
}

void VL_MungePic                (byte *source, unsigned width, unsigned height);
void VL_DrawPicBare             (int x, int y, byte *pic, int width, int height);
static inline void VL_MemToLatch              (byte *source, int width, int height,
                                    SDL_Surface *destSurface, int x, int y)
{
    if(destSurface == NULL)
        return;
    if (wolf3d_validate_mem_to_latch_bounds(x, y, width, height, destSurface->w, destSurface->h) != 0)
        Quit("VL_MemToLatch: Destination rectangle out of bounds!");
    byte *dest = VL_LockSurface(destSurface);
    if(dest == NULL)
        return;
    (void) wolf3d_copy_planar_to_linear((const uint8_t *) source, (uint8_t *) dest + y * destSurface->pitch + x, width, height, destSurface->pitch);
    VL_UnlockSurface(destSurface);
}
static inline void VL_ScreenToScreen          (SDL_Surface *source, SDL_Surface *dest)
{
    if(source != NULL && dest != NULL)
        (void) SDL_BlitSurface(source, NULL, dest, NULL);
}
static inline void VL_MemToScreenScaledCoord  (byte *source, int width, int height, int scx, int scy)
{
    if (wolf3d_validate_scaled_draw_bounds(scx, scy, width, height, scaleFactor, screenWidth, screenHeight) != 0)
        Quit("VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");
    byte *vbuf = VL_LockSurface(curSurface);
    if(vbuf == NULL)
        return;
    (void) wolf3d_draw_planar_scaled_to_linear((const uint8_t *) source, (uint8_t *) vbuf, width, height, scx, scy, scaleFactor, curPitch);
    VL_UnlockSurface(curSurface);
}
static inline void VL_MemToScreenScaledCoord  (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                    int destx, int desty, int width, int height)
{
    if (wolf3d_validate_scaled_draw_bounds(destx, desty, width, height, scaleFactor, screenWidth, screenHeight) != 0)
        Quit("VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");
    byte *vbuf = VL_LockSurface(curSurface);
    if(vbuf == NULL)
        return;
    (void) wolf3d_draw_planar_region_scaled_to_linear((const uint8_t *) source, (uint8_t *) vbuf, origwidth, origheight, srcx, srcy, destx, desty, width, height, scaleFactor, curPitch);
    VL_UnlockSurface(curSurface);
}

void inline VL_MemToScreen (byte *source, int width, int height, int x, int y)
{
    VL_MemToScreenScaledCoord(source, width, height, scaleFactor*x, scaleFactor*y);
}

void VL_MaskedToScreen (byte *source, int width, int height, int x, int y);

static inline void VL_LatchToScreenScaledCoord (SDL_Surface *source, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
    if(source == NULL)
        return;
    if (wolf3d_validate_scaled_draw_bounds(scxdest, scydest, width, height, scaleFactor, screenWidth, screenHeight) != 0)
        Quit("VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");
    byte *src = VL_LockSurface(source);
    byte *dest = VL_LockSurface(curSurface);
    if(src == NULL || dest == NULL)
    {
        if(dest != NULL)
            VL_UnlockSurface(curSurface);
        if(src != NULL)
            VL_UnlockSurface(source);
        return;
    }
    (void) wolf3d_copy_linear_scaled_to_linear((const uint8_t *) src, (uint8_t *) dest, xsrc, ysrc, width, height, scxdest, scydest, scaleFactor, source->pitch, curPitch);
    VL_UnlockSurface(curSurface);
    VL_UnlockSurface(source);
}

void inline VL_LatchToScreen (SDL_Surface *source, int xsrc, int ysrc,
    int width, int height, int xdest, int ydest)
{
    VL_LatchToScreenScaledCoord(source,xsrc,ysrc,width,height,
        scaleFactor*xdest,scaleFactor*ydest);
}
void inline VL_LatchToScreenScaledCoord (SDL_Surface *source, int scx, int scy)
{
    VL_LatchToScreenScaledCoord(source,0,0,source->w,source->h,scx,scy);
}
void inline VL_LatchToScreen (SDL_Surface *source, int x, int y)
{
    VL_LatchToScreenScaledCoord(source,0,0,source->w,source->h,
        scaleFactor*x,scaleFactor*y);
}
#endif

#endif

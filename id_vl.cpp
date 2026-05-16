// ID_VL.C

#define ID_VL_IMPLEMENTATION 1

#include <string.h>
#include "wl_def.h"
#include "crt.h"
#include "elisa_wolf3d_effects.h"
#include "elisa_wolf3d_sdl.h"
#include "elisa_wolf3d_video.h"
#pragma hdrstop

// Uncomment the following line, if you get destination out of bounds
// assertion errors and want to ignore them during debugging
//#define IGNORE_BAD_DEST

#ifdef IGNORE_BAD_DEST
#undef assert
#define assert(x) if(!(x)) return
#define assert_ret(x) if(!(x)) return 0
#else
#define assert_ret(x) assert(x)
#endif

boolean fullscreen = false;


boolean usedoublebuffering = true;
unsigned screenWidth = 640;
unsigned screenHeight = 480;
unsigned screenBits = -1;      // use "best" color depth according to libSDL


SDL_Surface *screen = NULL;
unsigned screenPitch;

SDL_Surface *screenBuffer = NULL;
unsigned bufferPitch;

SDL_Surface *curSurface = NULL;
unsigned curPitch;

SDL_Window *sdlWindow = NULL;
SDL_GLContext sdlGLContext = NULL;
SDL_Palette *sdlGamePalette = NULL;

unsigned scaleFactor;

boolean  screenfaded;
unsigned bordercolor;

SDL_Color palette1[256], palette2[256];
SDL_Color curpal[256];


#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 0}

SDL_Color gamepal[]={
#ifdef SPEAR
    #include "sodpal.inc"
#else
    #include "wolfpal.inc"
#endif
};

CASSERT(lengthof(gamepal) == 256)

//===========================================================================


/*
=======================
=
= VL_Shutdown
=
=======================
*/

void    VL_Shutdown (void)
{
    //VL_SetTextMode ();
}


/*
=======================
=
= VL_SetVGAPlaneMode
=
=======================
*/

void    VL_SetVGAPlaneMode (void)
{
#ifdef SPEAR
    const char *windowTitle = "Spear of Destiny";
#else
    const char *windowTitle = "Wolfenstein 3D";
#endif

    screenBits = 8;

    //Fab's CRT Hack
    //Adjust height so the screen is 4:3 aspect ratio
    screenHeight=screenWidth * 3.0/4.0;

    SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL;
    if(fullscreen)
        windowFlags |= SDL_WINDOW_FULLSCREEN;

    wolf3d_effect_video_window();
    sdlWindow = SDL_CreateWindow(windowTitle, screenWidth, screenHeight, windowFlags);
    if(!sdlWindow)
    {
        wolf3d_effect_diagnostics_console();
        printf("Unable to create %ix%i OpenGL window: %s\n", screenWidth, screenHeight, SDL_GetError());
        wolf3d_effect_process_abort();
        exit(1);
    }

    wolf3d_effect_video_window();
    sdlGLContext = SDL_GL_CreateContext(sdlWindow);
    if(!sdlGLContext)
    {
        wolf3d_effect_diagnostics_console();
        printf("Unable to create OpenGL context: %s\n", SDL_GetError());
        wolf3d_effect_process_abort();
        exit(1);
    }
    wolf3d_effect_video_window();
    SDL_GL_MakeCurrent(sdlWindow, sdlGLContext);
    SDL_GL_SetSwapInterval(usedoublebuffering ? 1 : 0);

    wolf3d_effect_input_grab();
    SDL_HideCursor();

    wolf3d_effect_video_palette();
    if(!sdlGamePalette)
    {
        sdlGamePalette = SDL_CreatePalette(256);
        if(!sdlGamePalette)
        {
            wolf3d_effect_diagnostics_console();
            printf("Unable to create palette: %s\n", SDL_GetError());
            wolf3d_effect_process_abort();
            exit(1);
        }
        wolf3d_sdl_set_palette_colors(sdlGamePalette, (uint8_t *) gamepal, 0, 256);
    }
    memcpy(curpal, gamepal, sizeof(SDL_Color) * 256);

    //Fab's CRT Hack
    CRT_Init(screenWidth);
    
    //Fab's CRT Hack
    screenWidth=320;
    screenHeight=200;

    if(!screen)
    {
        screen = SDL_CreateIndexedSurface(screenWidth, screenHeight);
        if(!screen)
        {
            wolf3d_effect_diagnostics_console();
            printf("Unable to create screen surface: %s\n", SDL_GetError());
            wolf3d_effect_process_abort();
            exit(1);
        }
    }

    if(!screenBuffer)
    {
        screenBuffer = SDL_CreateIndexedSurface(screenWidth, screenHeight);
        if(!screenBuffer)
        {
            wolf3d_effect_diagnostics_console();
            printf("Unable to create screen buffer surface: %s\n", SDL_GetError());
            wolf3d_effect_process_abort();
            exit(1);
        }
    }

    screenPitch = screen->pitch;
    bufferPitch = screenBuffer->pitch;

    curSurface = screenBuffer;
    curPitch = bufferPitch;

    scaleFactor = screenWidth/320;
    if(screenHeight/200 < scaleFactor) scaleFactor = screenHeight/200;
    
    
    pixelangle = (short *) malloc(screenWidth * sizeof(short));
    CHECKMALLOCRESULT(pixelangle);
    wallheight = (int *) malloc(screenWidth * sizeof(int));
    CHECKMALLOCRESULT(wallheight);
    
    
}

SDL_Surface *SDL_CreateIndexedSurface(int width, int height)
{
    wolf3d_effect_video_surface();
    SDL_Surface *surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_INDEX8);
    if(surface)
    {
        wolf3d_effect_video_palette();
        SDL_SetSurfacePalette(surface, sdlGamePalette);
    }
    return surface;
}

void SDL_SetGamePalette(SDL_Surface *surface, const SDL_Color *colors, int firstcolor, int ncolors)
{
    wolf3d_sdl_set_game_palette(surface, sdlGamePalette, (const uint8_t *) colors, firstcolor, ncolors);
}

void SDL_SetGamePaletteColor(SDL_Surface *surface, const SDL_Color *color, int firstcolor)
{
    SDL_SetGamePalette(surface, color, firstcolor, 1);
}

int SDL_NumJoysticksCompat(void)
{
    int count = 0;
    wolf3d_effect_input_poll();
    SDL_JoystickID *ids = SDL_GetJoysticks(&count);
    SDL_free(ids);
    return count;
}

SDL_Joystick *SDL_OpenJoystickByIndex(int index)
{
    int count = 0;
    wolf3d_effect_input_poll();
    SDL_JoystickID *ids = SDL_GetJoysticks(&count);
    SDL_Joystick *joystick = NULL;
    if(ids && index >= 0 && index < count)
    {
        wolf3d_effect_input_grab();
        joystick = SDL_OpenJoystick(ids[index]);
    }
    SDL_free(ids);
    return joystick;
}

/*
=============================================================================

                        PALETTE OPS

        To avoid snow, do a WaitVBL BEFORE calling these

=============================================================================
*/

/*
=================
=
= VL_ConvertPalette
=
=================
*/

void VL_ConvertPalette(byte *srcpal, SDL_Color *destpal, int numColors)
{
    wolf3d_convert_palette_6bit_rgb_to_sdl_colors(srcpal, (uint8_t *) destpal, numColors);
}

/*
=================
=
= VL_FillPalette
=
=================
*/

void VL_FillPalette (int red, int green, int blue)
{
    SDL_Color pal[256];

    wolf3d_fill_sdl_palette_rgb((uint8_t *) pal, red, green, blue, 256);
    VL_SetPalette(pal, true);
}

//===========================================================================

/*
=================
=
= VL_SetColor
=
=================
*/

void VL_SetColor    (int color, int red, int green, int blue)
{
    if (wolf3d_validate_palette_index(color) != 0)
        Quit ("VL_SetColor: color must be in the range [0, 255]!");

    SDL_Color col = { (Uint8)red, (Uint8)green, (Uint8)blue, 255 };
    curpal[color] = col;

    SDL_SetGamePaletteColor(curSurface, &col, color);
    wolf3d_effect_video_render();
    SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
    SDL_Flip(screen);
}

//===========================================================================

/*
=================
=
= VL_GetColor
=
=================
*/

void VL_GetColor    (int color, int *red, int *green, int *blue)
{
    if (wolf3d_validate_palette_index(color) != 0)
        Quit ("VL_GetColor: color must be in the range [0, 255]!");

    SDL_Color *col = &curpal[color];
    *red = col->r;
    *green = col->g;
    *blue = col->b;
}

//===========================================================================

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette (SDL_Color *palette, bool forceupdate)
{
    memcpy(curpal, palette, sizeof(SDL_Color) * 256);

    SDL_SetGamePalette(curSurface, palette, 0, 256);
    if(forceupdate)
    {
        wolf3d_effect_video_render();
        SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
        SDL_Flip(screen);
    }
}


//===========================================================================

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette (SDL_Color *palette)
{
    memcpy(palette, curpal, sizeof(SDL_Color) * 256);
}


//===========================================================================

/*
=================
=
= VL_FadeOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VL_FadeOut (int start, int end, int red, int green, int blue, int steps)
{
    if (wolf3d_vl_fade_out_from_host(start, end, red, green, blue, steps) != 0)
        Quit ("VL_FadeOut: steps must be greater than zero!");
    screenfaded = true;
}


/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn (int start, int end, SDL_Color *palette, int steps)
{
    if (wolf3d_vl_fade_in_from_host(start, end, (uint8_t *) palette, steps) != 0)
        Quit ("VL_FadeIn: steps must be greater than zero!");
    screenfaded = false;
}

/*
=============================================================================

                            PIXEL OPS

=============================================================================
*/

byte *VL_LockSurface(SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface))
    {
        if(!SDL_LockSurface(surface))
            return NULL;
    }
    return (byte *) surface->pixels;
}

void VL_UnlockSurface(SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
}

/*
=================
=
= VL_Plot
=
=================
*/

void VL_Plot (int x, int y, int color)
{
    if (wolf3d_validate_pixel_bounds(x, y, (int32_t) screenWidth, (int32_t) screenHeight) != 0)
        Quit ("VL_Plot: Pixel out of bounds!");
    VL_LockSurface(curSurface);
    ((byte *) curSurface->pixels)[y * curPitch + x] = color;
    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_GetPixel
=
=================
*/

byte VL_GetPixel (int x, int y)
{
    if (wolf3d_validate_pixel_bounds(x, y, (int32_t) screenWidth, (int32_t) screenHeight) != 0)
        Quit ("VL_GetPixel: Pixel out of bounds!");
    VL_LockSurface(curSurface);
    byte col = ((byte *) curSurface->pixels)[y * curPitch + x];
    VL_UnlockSurface(curSurface);
    return col;
}


/*
=================
=
= VL_Hlin
=
=================
*/

void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    if (wolf3d_validate_hline_bounds((int32_t) x, (int32_t) y, (int32_t) width, (int32_t) screenWidth, (int32_t) screenHeight) != 0)
        Quit ("VL_Hlin: Destination rectangle out of bounds!");
    VL_LockSurface(curSurface);
    Uint8 *dest = ((byte *) curSurface->pixels) + y * curPitch + x;
    wolf3d_fill_linear_span(dest, width, color);
    VL_UnlockSurface(curSurface);
}


/*
=================
=
= VL_Vlin
=
=================
*/

void VL_Vlin (int x, int y, int height, int color)
{
    if (wolf3d_validate_vline_bounds(x, y, height, (int32_t) screenWidth, (int32_t) screenHeight) != 0)
        Quit ("VL_Vlin: Destination rectangle out of bounds!");
    VL_LockSurface(curSurface);
    Uint8 *dest = ((byte *) curSurface->pixels) + y * curPitch + x;
    wolf3d_fill_linear_vline(dest, height, curPitch, color);
    VL_UnlockSurface(curSurface);
}


/*
=================
=
= VL_Bar
=
=================
*/

void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
    if (wolf3d_validate_bar_bounds(scx, scy, scwidth, scheight, (int32_t) screenWidth, (int32_t) screenHeight) != 0)
        Quit ("VL_BarScaledCoord: Destination rectangle out of bounds!");
    VL_LockSurface(curSurface);
    Uint8 *dest = ((byte *) curSurface->pixels) + scy * curPitch + scx;
    wolf3d_fill_linear_rect(dest, scwidth, scheight, curPitch, color);
    VL_UnlockSurface(curSurface);
}

/*
============================================================================

                            MEMORY OPS

============================================================================
*/

/*
=================
=
= VL_MemToLatch
=
=================
*/

void VL_MemToLatch(byte *source, int width, int height,
    SDL_Surface *destSurface, int x, int y)
{
    if (wolf3d_validate_mem_to_latch_bounds(x, y, width, height, (int32_t) destSurface->w, (int32_t) destSurface->h) != 0)
        Quit ("VL_MemToLatch: Destination rectangle out of bounds!");

    VL_LockSurface(destSurface);
    int pitch = destSurface->pitch;
    byte *dest = (byte *) destSurface->pixels + y * pitch + x;
    wolf3d_copy_planar_to_linear(source, dest, width, height, pitch);
    VL_UnlockSurface(destSurface);
}

//===========================================================================


/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a block of data to the screen with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int width, int height, int destx, int desty)
{
    if (wolf3d_validate_scaled_draw_bounds(destx, desty, width, height, scaleFactor, screenWidth, screenHeight) != 0)
        Quit ("VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");
    VL_LockSurface(curSurface);
    byte *vbuf = (byte *) curSurface->pixels;
    wolf3d_draw_planar_scaled_to_linear(source, vbuf, width, height, destx, desty, scaleFactor, curPitch);
    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a part of a block of data to the screen.
= The block has the size origwidth*origheight.
= The part at (srcx, srcy) has the size width*height
= and will be painted to (destx, desty) with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord (byte *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    if (wolf3d_validate_scaled_draw_bounds(destx, desty, width, height, scaleFactor, screenWidth, screenHeight) != 0)
        Quit ("VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");
    VL_LockSurface(curSurface);
    byte *vbuf = (byte *) curSurface->pixels;
    wolf3d_draw_planar_region_scaled_to_linear(source, vbuf, origwidth, origheight, srcx, srcy,
        destx, desty, width, height, scaleFactor, curPitch);
    VL_UnlockSurface(curSurface);
}

//==========================================================================

/*
=================
=
= VL_LatchToScreen
=
=================
*/

void VL_LatchToScreenScaledCoord(SDL_Surface *source, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
    if (wolf3d_validate_scaled_draw_bounds(scxdest, scydest, width, height, scaleFactor, screenWidth, screenHeight) != 0)
        Quit ("VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");

    if(scaleFactor == 1 && screenBits == 8)
    {
        SDL_Rect srcrect = { xsrc, ysrc, width, height };
        SDL_Rect destrect = { scxdest, scydest, 0, 0 }; // width and height are ignored
        SDL_BlitSurface(source, &srcrect, curSurface, &destrect);
    }
    else
    {
        VL_LockSurface(source);
        byte *src = (byte *) source->pixels;
        unsigned srcPitch = source->pitch;

        VL_LockSurface(curSurface);
        byte *vbuf = (byte *) curSurface->pixels;
        wolf3d_copy_linear_scaled_to_linear(src, vbuf, xsrc, ysrc, width, height,
            scxdest, scydest, scaleFactor, srcPitch, curPitch);
        VL_UnlockSurface(curSurface);
        VL_UnlockSurface(source);
    }
}

//===========================================================================

/*
=================
=
= VL_ScreenToScreen
=
=================
*/

void VL_ScreenToScreen (SDL_Surface *source, SDL_Surface *dest)
{
    SDL_BlitSurface(source, NULL, dest, NULL);
}

extern "C" int wolf3d_legacy_vl_set_vga_plane_mode(void)
{
    VL_SetVGAPlaneMode();
    return 0;
}

extern "C" SDL_Surface *wolf3d_legacy_sdl_create_indexed_surface(int width, int height)
{
    return SDL_CreateIndexedSurface(width, height);
}

extern "C" int wolf3d_legacy_sdl_set_game_palette(SDL_Surface *surface, const SDL_Color *colors, int firstcolor, int ncolors)
{
    SDL_SetGamePalette(surface, colors, firstcolor, ncolors);
    return 0;
}

extern "C" int wolf3d_legacy_copy_game_palette(uint8_t *dest)
{
    memcpy(dest, gamepal, sizeof(gamepal));
    return 0;
}

extern "C" int wolf3d_legacy_sdl_set_game_palette_color(SDL_Surface *surface, const SDL_Color *color, int firstcolor)
{
    SDL_SetGamePaletteColor(surface, color, firstcolor);
    return 0;
}

extern "C" int wolf3d_legacy_install_vl_bootstrap(SDL_Palette *palette, SDL_Surface *screenSurface, SDL_Surface *screenBufferSurface)
{
    sdlGamePalette = palette;
    screen = screenSurface;
    screenBuffer = screenBufferSurface;
    screenPitch = screen ? screen->pitch : 0;
    bufferPitch = screenBuffer ? screenBuffer->pitch : 0;
    curSurface = screenBuffer;
    curPitch = bufferPitch;
    return 0;
}

extern "C" int wolf3d_legacy_set_screen_width(int value)
{
    screenWidth = (unsigned) value;
    return 0;
}

extern "C" int wolf3d_legacy_set_screen_height(int value)
{
    screenHeight = (unsigned) value;
    return 0;
}

extern "C" int wolf3d_legacy_set_screen_bits(int value)
{
    screenBits = (unsigned) value;
    return 0;
}

extern "C" int wolf3d_legacy_set_usedoublebuffering(int value)
{
    usedoublebuffering = value != 0;
    return 0;
}

extern "C" int wolf3d_legacy_sdl_num_joysticks_compat(void)
{
    return SDL_NumJoysticksCompat();
}

extern "C" SDL_Joystick *wolf3d_legacy_sdl_open_joystick_by_index(int index)
{
    return SDL_OpenJoystickByIndex(index);
}

extern "C" int wolf3d_legacy_vl_set_palette(SDL_Color *palette, int forceupdate)
{
    VL_SetPalette(palette, forceupdate != 0);
    return 0;
}

extern "C" int wolf3d_legacy_vl_get_palette(SDL_Color *palette)
{
    VL_GetPalette(palette);
    return 0;
}

extern "C" int wolf3d_legacy_vl_fill_palette(int red, int green, int blue)
{
    VL_FillPalette(red, green, blue);
    return 0;
}

extern "C" int wolf3d_legacy_vl_fade_out(int start, int end, int red, int green, int blue, int steps)
{
    VL_FadeOut(start, end, red, green, blue, steps);
    return 0;
}

extern "C" int wolf3d_legacy_vl_fade_in(int start, int end, SDL_Color *palette, int steps)
{
    VL_FadeIn(start, end, palette, steps);
    return 0;
}

extern "C" int wolf3d_legacy_vl_plot(int x, int y, int color)
{
    VL_Plot(x, y, color);
    return 0;
}

extern "C" int wolf3d_legacy_vl_get_pixel(int x, int y)
{
    return (int) VL_GetPixel(x, y);
}

extern "C" int wolf3d_legacy_vl_hlin(int x, int y, int width, int color)
{
    VL_Hlin((unsigned) x, (unsigned) y, (unsigned) width, color);
    return 0;
}

extern "C" int wolf3d_legacy_vl_vlin(int x, int y, int height, int color)
{
    VL_Vlin(x, y, height, color);
    return 0;
}

extern "C" int wolf3d_legacy_vl_bar_scaled_coord(int scx, int scy, int scwidth, int scheight, int color)
{
    VL_BarScaledCoord(scx, scy, scwidth, scheight, color);
    return 0;
}

extern "C" int wolf3d_legacy_vl_mem_to_latch(byte *source, int width, int height, SDL_Surface *destSurface, int x, int y)
{
    VL_MemToLatch(source, width, height, destSurface, x, y);
    return 0;
}

extern "C" int wolf3d_legacy_vl_mem_to_screen_scaled_coord(byte *source, int width, int height, int destx, int desty)
{
    VL_MemToScreenScaledCoord(source, width, height, destx, desty);
    return 0;
}

extern "C" int wolf3d_legacy_vl_mem_to_screen_scaled_coord_region(byte *source, int origwidth, int origheight, int srcx, int srcy, int destx, int desty, int width, int height)
{
    VL_MemToScreenScaledCoord(source, origwidth, origheight, srcx, srcy, destx, desty, width, height);
    return 0;
}

extern "C" int wolf3d_legacy_vl_latch_to_screen_scaled_coord(SDL_Surface *source, int xsrc, int ysrc, int width, int height, int scxdest, int scydest)
{
    VL_LatchToScreenScaledCoord(source, xsrc, ysrc, width, height, scxdest, scydest);
    return 0;
}

extern "C" int wolf3d_legacy_vl_screen_to_screen(SDL_Surface *source, SDL_Surface *dest)
{
    VL_ScreenToScreen(source, dest);
    return 0;
}

extern "C" int wolf3d_legacy_get_scale_factor(void)
{
    return (int) scaleFactor;
}

extern "C" int wolf3d_legacy_get_usedoublebuffering(void)
{
    return usedoublebuffering ? 1 : 0;
}

extern "C" int wolf3d_legacy_get_screen_bits(void)
{
    return (int) screenBits;
}

extern "C" int wolf3d_legacy_get_screen_width(void)
{
    return (int) screenWidth;
}

extern "C" int wolf3d_legacy_get_screen_height(void)
{
    return (int) screenHeight;
}

extern "C" int wolf3d_legacy_get_screen_pitch(void)
{
    return (int) screenPitch;
}

extern "C" int wolf3d_legacy_get_buffer_pitch(void)
{
    return (int) bufferPitch;
}

extern "C" int wolf3d_legacy_get_cur_pitch(void)
{
    return (int) curPitch;
}

extern "C" void *wolf3d_legacy_get_screen_surface(void)
{
    return screen;
}

extern "C" void *wolf3d_legacy_get_screen_buffer_surface(void)
{
    return screenBuffer;
}

extern "C" void *wolf3d_legacy_get_cur_surface(void)
{
    return curSurface;
}

extern "C" void *wolf3d_legacy_get_game_palette(void)
{
    return sdlGamePalette;
}

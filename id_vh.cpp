#include "wl_def.h"
#include "elisa_wolf3d_video.h"


pictabletype    *pictable;
SDL_Surface     *latchpics[NUMLATCHPICS];

int     px,py;
byte    fontcolor,backcolor;
int     fontnumber;

//==========================================================================

void VWB_DrawPropString(const char* string)
{
    fontstruct  *font;
    byte        *dest;

    byte *vbuf = LOCK();

    font = (fontstruct *) grsegs[STARTFONT+fontnumber];
    dest = vbuf + scaleFactor * (py * curPitch + px);
    px += wolf3d_vwb_draw_prop_string_on_linear_buffer((const uint8_t *) string, (const uint8_t *) font,
        font->location, (const uint8_t *) font->width, dest, font->height, scaleFactor,
        curPitch, fontcolor);

    UNLOCK();
}

/*
=================
=
= VL_MungePic
=
=================
*/

void VL_MungePic (byte *source, unsigned width, unsigned height)
{
    unsigned size;
    byte *temp;

    size = width*height;

    if (wolf3d_validate_munge_pic_width(width) != 0)
        Quit ("VL_MungePic: Not divisable by 4!");

//
// copy the pic to a temp buffer
//
    temp=(byte *) malloc(size);
    CHECKMALLOCRESULT(temp);
    memcpy (temp,source,size);

//
// munge it back into the original buffer
//
    wolf3d_munge_pic_from_temp(source, temp, width, height);

    free(temp);
}

void VWL_MeasureString (const char *string, word *width, word *height, fontstruct *font)
{
    if (wolf3d_measure_prop_string_safe((const uint8_t *) string,
        width, height, font ? font->height : -1, font ? (const uint8_t *) font->width : nullptr) != 0)
        Quit ("VWL_MeasureString: Invalid font height!");
}

void VW_MeasurePropString (const char *string, word *width, word *height)
{
    VWL_MeasureString(string,width,height,(fontstruct *)grsegs[STARTFONT+fontnumber]);
}

/*
=============================================================================

                Double buffer management routines

=============================================================================
*/

void VH_UpdateScreen()
{
    wolf3d_vh_update_screen(screenBuffer, screen);
}


void VWB_DrawTile8 (int x, int y, int tile)
{
    LatchDrawChar(x,y,tile);
}

void VWB_DrawTile8M (int x, int y, int tile)
{
    VL_MemToScreen (((byte *)grsegs[STARTTILE8M])+tile*64,8,8,x,y);
}

void VWB_DrawPic (int x, int y, int chunknum)
{
    int picnum = wolf3d_pic_index_for_chunk(chunknum, STARTPICS);
    unsigned width,height;
    byte *vbuf;

    x = wolf3d_align_picture_x(x);

    width = pictable[picnum].width;
    height = pictable[picnum].height;

    vbuf = LOCK();
    wolf3d_vwb_draw_pic_on_linear_buffer((const uint8_t *) grsegs[chunknum], (int32_t) width, (int32_t) height,
        vbuf, x, y, curPitch, scaleFactor, (int32_t) screenWidth, (int32_t) screenHeight);
    UNLOCK();
}

void VWB_DrawPicScaledCoord (int scx, int scy, int chunknum)
{
    int picnum = wolf3d_pic_index_for_chunk(chunknum, STARTPICS);
    unsigned width,height;
    byte *vbuf;

    width = pictable[picnum].width;
    height = pictable[picnum].height;

    vbuf = LOCK();
    wolf3d_vwb_draw_pic_scaled_coord_on_linear_buffer((const uint8_t *) grsegs[chunknum], (int32_t) width, (int32_t) height,
        vbuf, scx, scy, curPitch, scaleFactor, (int32_t) screenWidth, (int32_t) screenHeight);
    UNLOCK();
}


extern "C" int wolf3d_legacy_register_latchpic_surface(int index, SDL_Surface *surface)
{
    latchpics[index] = surface;
    return 0;
}

extern "C" SDL_Surface *wolf3d_legacy_get_latchpic_surface(int index)
{
    return latchpics[index];
}

extern "C" int wolf3d_legacy_get_latchpic_dimensions(int picnum, int *width, int *height)
{
    int picindex = wolf3d_pic_index_for_chunk(picnum, STARTPICS);
    *width = pictable[picindex].width;
    *height = pictable[picindex].height;
    return 0;
}

void VWB_Bar (int x, int y, int width, int height, int color)
{
    byte *vbuf = LOCK();
    wolf3d_vwb_bar_on_linear_buffer(vbuf, x, y, width, height, color, curPitch, scaleFactor, (int32_t) screenWidth, (int32_t) screenHeight);
    UNLOCK();
}

void VWB_Plot (int x, int y, int color)
{
    byte *vbuf = LOCK();
    wolf3d_vwb_plot_on_linear_buffer(vbuf, x, y, color, curPitch, scaleFactor, (int32_t) screenWidth, (int32_t) screenHeight);
    UNLOCK();
}

void VWB_Hlin (int x1, int x2, int y, int color)
{
    byte *vbuf = LOCK();
    wolf3d_vwb_hlin_on_linear_buffer(vbuf, x1, x2, y, color, curPitch, scaleFactor, (int32_t) screenWidth, (int32_t) screenHeight);
    UNLOCK();
}

void VWB_Vlin (int y1, int y2, int x, int color)
{
    byte *vbuf = LOCK();
    wolf3d_vwb_vlin_on_linear_buffer(vbuf, y1, y2, x, color, curPitch, scaleFactor, (int32_t) screenWidth, (int32_t) screenHeight);
    UNLOCK();
}


/*
=============================================================================

                        WOLFENSTEIN STUFF

=============================================================================
*/

/*
=====================
=
= LatchDrawPic
=
=====================
*/

void LatchDrawPic (unsigned x, unsigned y, unsigned picnum)
{
    if (wolf3d_latch_draw_pic_on_screen((int32_t) picnum, (int32_t) x, (int32_t) y) != 0)
        Quit ("LatchDrawPic: Unable to draw latch pic!");
}

void LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum)
{
    if (wolf3d_latch_draw_pic_scaled_coord_on_screen((int32_t) picnum, (int32_t) scx, (int32_t) scy) != 0)
        Quit ("LatchDrawPicScaledCoord: Unable to draw latch pic!");
}


//==========================================================================

/*
===================
=
= LoadLatchMem
=
===================
*/

void LoadLatchMem (void)
{
    int i,width,height,start,end;
    byte *src;
    SDL_Surface *surf;

//
// tile 8s
//
    
    surf = latchpics[0];
    if(surf == NULL)
    {
        surf = SDL_CreateIndexedSurface(8*8, wolf3d_latch_tile_surface_height(NUMTILE8));
        if(surf == NULL)
        {
            Quit("Unable to create surface for tiles!");
        }
        SDL_SetGamePalette(surf, gamepal, 0, 256);
    }

    latchpics[0] = surf;
    CA_CacheGrChunk (STARTTILE8);
    src = grsegs[STARTTILE8];

    for (i=0;i<NUMTILE8;i++)
    {
        VL_MemToLatch (src, 8, 8, surf, wolf3d_latch_tile_dest_x(i), wolf3d_latch_tile_dest_y(i));
        src += 64;
    }
    UNCACHEGRCHUNK (STARTTILE8);

//
// pics
//
    start = LATCHPICS_LUMP_START;
    end = LATCHPICS_LUMP_END;

    for (i=start;i<=end;i++)
    {
        width = pictable[i-STARTPICS].width;
        height = pictable[i-STARTPICS].height;
        surf = latchpics[wolf3d_latchpic_index(i, start)];
        if(surf == NULL)
        {
            surf = SDL_CreateIndexedSurface(width, height);
            if(surf == NULL)
            {
                Quit("Unable to create surface for picture!");
            }
            SDL_SetGamePalette(surf, gamepal, 0, 256);
        }

        latchpics[wolf3d_latchpic_index(i, start)] = surf;
        CA_CacheGrChunk (i);
        VL_MemToLatch (grsegs[i], width, height, surf, 0, 0);
        UNCACHEGRCHUNK(i);
    }
}

//==========================================================================

/*
===================
=
= FizzleFade
=
= returns true if aborted
=
= It uses maximum-length Linear Feedback Shift Registers (LFSR) counters.
= You can find a list of them with lengths from 3 to 168 at:
= http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
= Many thanks to Xilinx for this list!!!
=
===================
*/

static unsigned int rndbits_y;
static unsigned int rndmask;

extern SDL_Color curpal[256];

void VH_Startup()
{
    wolf3d_configure_fizzle_state((uint32_t)screenWidth, (uint32_t)screenHeight, &rndbits_y, &rndmask);
}

extern "C" int wolf3d_legacy_vwb_draw_prop_string(const char* string)
{
    VWB_DrawPropString(string);
    return 0;
}

extern "C" int wolf3d_legacy_vwb_draw_pic(int x, int y, int chunknum)
{
    VWB_DrawPic(x, y, chunknum);
    return 0;
}

extern "C" int wolf3d_legacy_vwb_draw_pic_scaled_coord(int scx, int scy, int chunknum)
{
    VWB_DrawPicScaledCoord(scx, scy, chunknum);
    return 0;
}

extern "C" int wolf3d_legacy_vwb_bar(int x, int y, int width, int height, int color)
{
    VWB_Bar(x, y, width, height, color);
    return 0;
}

extern "C" int wolf3d_legacy_vwb_plot(int x, int y, int color)
{
    VWB_Plot(x, y, color);
    return 0;
}

extern "C" int wolf3d_legacy_vwb_hlin(int x1, int x2, int y, int color)
{
    VWB_Hlin(x1, x2, y, color);
    return 0;
}

boolean FizzleFade (SDL_Surface *source, int x1, int y1,
    unsigned width, unsigned height, unsigned frames, boolean abortable)
{
    SDL_Surface *source_copy = SDL_DuplicateSurface(source);
    SDL_Surface *screen_copy = SDL_DuplicateSurface(screen);
    if(!source_copy || !screen_copy)
    {
        SDL_FreeSurface(source_copy);
        SDL_FreeSurface(screen_copy);
        Quit("FizzleFade: Unable to duplicate surfaces!");
    }

    byte *srcptr = VL_LockSurface(source_copy);
    byte *destptr = VL_LockSurface(screen_copy);
    boolean aborted = wolf3d_fizzle_fade_on_locked_buffers(srcptr, (int32_t) source_copy->pitch, destptr,
        (int32_t) screen_copy->pitch, x1, y1, (int32_t) width, (int32_t) height, (uint32_t) frames,
        abortable ? 1 : 0, usedoublebuffering ? 1 : 0, (int32_t) rndbits_y, rndmask, (int32_t) source_copy->w,
        (int32_t) source_copy->h, (int32_t) screen->w, (int32_t) screen->h, source_copy, screenBuffer, screen);
    VL_UnlockSurface(screen_copy);
    VL_UnlockSurface(source_copy);
    SDL_FreeSurface(source_copy);
    SDL_FreeSurface(screen_copy);
    return aborted;
}

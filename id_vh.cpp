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
    int         width, step, height;
    byte        *source, *dest;
    byte        ch;

    byte *vbuf = LOCK();

    font = (fontstruct *) grsegs[STARTFONT+fontnumber];
    height = font->height;
    dest = vbuf + scaleFactor * (py * curPitch + px);

    while ((ch = (byte)*string++)!=0)
    {
        width = step = font->width[ch];
        source = ((byte *)font)+font->location[ch];
        while (width--)
        {
            for(int i=0;i<height;i++)
            {
                if(source[i*step])
                {
                    for(unsigned sy=0; sy<scaleFactor; sy++)
                        for(unsigned sx=0; sx<scaleFactor; sx++)
                            dest[(scaleFactor*i+sy)*curPitch+sx]=fontcolor;
                }
            }

            source++;
            px++;
            dest+=scaleFactor;
        }
    }

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
    *height = font->height;
    *width = (word) wolf3d_measure_prop_string_width((uint8_t *) string, (uint8_t *) font->width);
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
    SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
    SDL_Flip(screen);
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
    int picnum = chunknum - STARTPICS;
    unsigned width,height;

    x &= ~7;

    width = pictable[picnum].width;
    height = pictable[picnum].height;

    VL_MemToScreen (grsegs[chunknum],width,height,x,y);
}

void VWB_DrawPicScaledCoord (int scx, int scy, int chunknum)
{
    int picnum = chunknum - STARTPICS;
    unsigned width,height;

    width = pictable[picnum].width;
    height = pictable[picnum].height;

    VL_MemToScreenScaledCoord (grsegs[chunknum],width,height,scx,scy);
}


void VWB_Bar (int x, int y, int width, int height, int color)
{
    VW_Bar (x,y,width,height,color);
}

void VWB_Plot (int x, int y, int color)
{
    if(scaleFactor == 1)
        VW_Plot(x,y,color);
    else
        VW_Bar(x, y, 1, 1, color);
}

void VWB_Hlin (int x1, int x2, int y, int color)
{
    int width = (int) wolf3d_inclusive_span_length((int32_t) x1, (int32_t) x2);

    if (width < 0)
        Quit("VWB_Hlin: invalid line span");

    if(scaleFactor == 1)
        VW_Hlin(x1,x2,y,color);
    else
        VW_Bar(x1, y, width, 1, color);
}

void VWB_Vlin (int y1, int y2, int x, int color)
{
    int height = (int) wolf3d_inclusive_span_length((int32_t) y1, (int32_t) y2);

    if (height < 0)
        Quit("VWB_Vlin: invalid line span");

    if(scaleFactor == 1)
        VW_Vlin(y1,y2,x,color);
    else
        VW_Bar(x, y1, 1, height, color);
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
    VL_LatchToScreen (latchpics[2+picnum-LATCHPICS_LUMP_START], x*8, y);
}

void LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum)
{
    VL_LatchToScreenScaledCoord (latchpics[2+picnum-LATCHPICS_LUMP_START], scx*8, scy);
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
    
    surf = SDL_CreateIndexedSurface(8*8, ((NUMTILE8 + 7) / 8) * 8);
    if(surf == NULL)
    {
        Quit("Unable to create surface for tiles!");
    }
    SDL_SetGamePalette(surf, gamepal, 0, 256);

    latchpics[0] = surf;
    CA_CacheGrChunk (STARTTILE8);
    src = grsegs[STARTTILE8];

    for (i=0;i<NUMTILE8;i++)
    {
        VL_MemToLatch (src, 8, 8, surf, (i & 7) * 8, (i >> 3) * 8);
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
        surf = SDL_CreateIndexedSurface(width, height);
        if(surf == NULL)
        {
            Quit("Unable to create surface for picture!");
        }
        SDL_SetGamePalette(surf, gamepal, 0, 256);

        latchpics[2+i-start] = surf;
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
    int rndbits = wolf3d_fizzle_bits_for_dimensions((uint32_t)screenWidth, (uint32_t)screenHeight);
    rndbits_y = wolf3d_log2_ceil((uint32_t)screenHeight);
    rndmask = wolf3d_fizzle_mask_for_dimensions((uint32_t)screenWidth, (uint32_t)screenHeight);
}

boolean FizzleFade (SDL_Surface *source, int x1, int y1,
    unsigned width, unsigned height, unsigned frames, boolean abortable)
{

    unsigned x, y, frame, pixperframe;
    int32_t  rndval, lastrndval;
    int      first = 1;

    lastrndval = 0;
    if (wolf3d_validate_fizzle_work_area(x1, y1, (int) width, (int) height, (int) source->w, (int) source->h, (int) screenWidth, (int) screenHeight) != 0)
        Quit ("FizzleFade: rectangle out of bounds!");

    if (wolf3d_validate_fizzle_frames(frames) != 0)
        Quit ("FizzleFade: frames must be greater than zero!");
    pixperframe = width * height / frames;

    IN_StartAck ();

    frame = GetTimeCount();

    //can't rely on screen as dest b/c crt.cpp writes over it with screenBuffer
    //can't rely on screenBuffer as source for same reason: every flip it has to be updated
    SDL_Surface *source_copy = SDL_DuplicateSurface(source);
    SDL_Surface *screen_copy = SDL_DuplicateSurface(screen);

    byte *srcptr = VL_LockSurface(source_copy);
    do
    {
        if(abortable && IN_CheckAck ())
        {
            VL_UnlockSurface(source_copy);
            SDL_BlitSurface(screen_copy, NULL, screenBuffer, NULL);
            SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
            SDL_Flip(screen);
            SDL_FreeSurface(source_copy);
            SDL_FreeSurface(screen_copy);
            return true;
        }

        byte *destptr = VL_LockSurface(screen_copy);

        rndval = lastrndval;

        // When using double buffering, we have to copy the pixels of the last AND the current frame.
        // Only for the first frame, there is no "last frame"
        for(int i = first; i < 2; i++)
        {
            for(unsigned p = 0; p < pixperframe; p++)
            {
                //
                // seperate random value into x/y pair
                //

                x = rndval >> rndbits_y;
                y = rndval & ((1 << rndbits_y) - 1);

                //
                // advance to next random element
                //

                rndval = (rndval >> 1) ^ (rndval & 1 ? 0 : rndmask);

                if(x >= width || y >= height)
                {
                    if(rndval == 0)     // entire sequence has been completed
                        goto finished;
                    p--;
                    continue;
                }

                //
                // copy one pixel
                //

                if(screenBits == 8)
                {
                    *(destptr + (y1 + y) * screen->pitch + x1 + x)
                        = *(srcptr + (y1 + y) * source->pitch + x1 + x);
                }
                else
                {
                    byte col = *(srcptr + (y1 + y) * source->pitch + x1 + x);
                    *(destptr + (y1 + y) * screen->pitch + x1 + x) = col;
                }

                if(rndval == 0)     // entire sequence has been completed
                    goto finished;
            }

            if(!i || first) lastrndval = rndval;
        }

        // If there is no double buffering, we always use the "first frame" case
        if(usedoublebuffering) first = 0;

        VL_UnlockSurface(screen_copy);
        SDL_BlitSurface(screen_copy, NULL, screenBuffer, NULL);
        SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
        SDL_Flip(screen);

        frame++;
        Delay(frame - GetTimeCount());        // don't go too fast
    } while (1);

finished:
    VL_UnlockSurface(source_copy);
    VL_UnlockSurface(screen_copy);
    SDL_BlitSurface(screen_copy, NULL, screenBuffer, NULL);
    SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_FreeSurface(source_copy);
    SDL_FreeSurface(screen_copy);
    return false;
}

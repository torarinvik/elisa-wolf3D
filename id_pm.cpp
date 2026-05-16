#include "wl_def.h"
#include "elisa_wolf3d_effects.h"
#include "elisa_wolf3d_pagefile.h"

int ChunksInFile;
int PMSpriteStart;
int PMSoundStart;

bool PMSoundInfoPagePadded = false;

// holds the whole VSWAP
uint32_t *PMPageData;
size_t PMPageDataSize;

// ChunksInFile+1 pointers to page starts.
// The last pointer points one byte after the last page.
uint8_t **PMPages;

void PM_Startup()
{
    char fname[13] = "vswap.";
    strcat(fname,extension);

    wolf3d_effect_filesystem_read();
    FILE *file = fopen(fname,"rb");
    if(!file)
        CA_CannotOpen(fname);

    ChunksInFile = 0;
    wolf3d_effect_filesystem_read();
    fread(&ChunksInFile, sizeof(word), 1, file);
    PMSpriteStart = 0;
    wolf3d_effect_filesystem_read();
    fread(&PMSpriteStart, sizeof(word), 1, file);
    PMSoundStart = 0;
    wolf3d_effect_filesystem_read();
    fread(&PMSoundStart, sizeof(word), 1, file);

    uint32_t* pageOffsets = (uint32_t *) malloc((ChunksInFile + 1) * sizeof(int32_t));
    CHECKMALLOCRESULT(pageOffsets);
    wolf3d_effect_filesystem_read();
    fread(pageOffsets, sizeof(uint32_t), ChunksInFile, file);

    word *pageLengths = (word *) malloc(ChunksInFile * sizeof(word));
    CHECKMALLOCRESULT(pageLengths);
    wolf3d_effect_filesystem_read();
    fread(pageLengths, sizeof(word), ChunksInFile, file);

    wolf3d_effect_filesystem_read();
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    long pageDataSize = fileSize - pageOffsets[0];
    if(pageDataSize > (size_t) -1)
        Quit("The page file \"%s\" is too large!", fname);

    pageOffsets[ChunksInFile] = fileSize;

    uint32_t dataStart = pageOffsets[0];
    int i;

    // Check that all pageOffsets are valid
    for(i = 0; i < ChunksInFile; i++)
    {
        if(!pageOffsets[i]) continue;   // sparse page
        if(wolf3d_validate_page_offset(pageOffsets[i], dataStart, (uint32_t) fileSize) != 0)
            Quit("Illegal page offset for page %i: %u (filesize: %u)",
                    i, pageOffsets[i], fileSize);
    }

    // Calculate total amount of padding needed for sprites and sound info page
    int alignPadding = 0;
    for(i = PMSpriteStart; i < PMSoundStart; i++)
    {
        if(!pageOffsets[i]) continue;   // sparse page
        uint32_t offs = pageOffsets[i] - dataStart + alignPadding;
        if(offs & 1)
            alignPadding++;
    }

    if((pageOffsets[ChunksInFile - 1] - dataStart + alignPadding) & 1)
        alignPadding++;

    PMPageDataSize = (size_t) pageDataSize + alignPadding;
    PMPageData = (uint32_t *) malloc(PMPageDataSize);
    CHECKMALLOCRESULT(PMPageData);

    PMPages = (uint8_t **) malloc((ChunksInFile + 1) * sizeof(uint8_t *));
    CHECKMALLOCRESULT(PMPages);

    // Load pages and initialize PMPages pointers
    uint8_t *ptr = (uint8_t *) PMPageData;
    for(i = 0; i < ChunksInFile; i++)
    {
        if(i >= PMSpriteStart && i < PMSoundStart || i == ChunksInFile - 1)
        {
            size_t offs = ptr - (uint8_t *) PMPageData;

            // pad with zeros to make it 2-byte aligned
            if(offs & 1)
            {
                *ptr++ = 0;
                if(i == ChunksInFile - 1) PMSoundInfoPagePadded = true;
            }
        }

        PMPages[i] = ptr;

        if(!pageOffsets[i])
            continue;               // sparse page

        // Use specified page length, when next page is sparse page.
        // Otherwise, calculate size from the offset difference between this and the next page.
        uint32_t size;
        if(!pageOffsets[i + 1]) size = pageLengths[i];
        else size = pageOffsets[i + 1] - pageOffsets[i];

        wolf3d_effect_filesystem_read();
        fseek(file, pageOffsets[i], SEEK_SET);
        fread(ptr, 1, size, file);
        ptr += size;
    }

    // last page points after page buffer
    PMPages[ChunksInFile] = ptr;

    free(pageLengths);
    free(pageOffsets);
    wolf3d_effect_filesystem_read();
    fclose(file);
}

void PM_Shutdown()
{
    free(PMPages);
    free(PMPageData);
}

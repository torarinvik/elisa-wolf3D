#ifndef __ID_CA__
#define __ID_CA__

//===========================================================================

#define NUMMAPS         60
#define MAPPLANES       2

#define UNCACHEGRCHUNK(chunk) {if(grsegs[chunk]) {free(grsegs[chunk]); grsegs[chunk]=NULL;}}
#define UNCACHEAUDIOCHUNK(chunk) {if(audiosegs[chunk]) {free(audiosegs[chunk]); audiosegs[chunk]=NULL;}}

//===========================================================================

typedef struct
{
    int32_t planestart[3];
    uint16_t    planelength[3];
    uint16_t    width,height;
    char    name[16];
} maptype;

//===========================================================================

extern  int   mapon;

extern  uint16_t *mapsegs[MAPPLANES];
extern  uint8_t *audiosegs[NUMSNDCHUNKS];
extern  uint8_t *grsegs[NUMCHUNKS];

extern  char  extension[5];
extern  char  graphext[5];
extern  char  audioext[5];

//===========================================================================

int8_t CA_LoadFile (const char *filename, void* *ptr);
int8_t CA_WriteFile (const char *filename, void *ptr, int32_t length);

int32_t CA_RLEWCompress (uint16_t *source, int32_t length, uint16_t *dest, uint16_t rlewtag);

void CA_RLEWexpand (uint16_t *source, uint16_t *dest, int32_t length, uint16_t rlewtag);

void CA_Startup (void);
void CA_Shutdown (void);

int32_t CA_CacheAudioChunk (int chunk);
void CA_LoadAllSounds (void);

void CA_CacheGrChunk (int chunk);
void CA_CacheMap (int mapnum);

void CA_CacheScreen (int chunk);

void CA_CannotOpen(const char *name);

#endif

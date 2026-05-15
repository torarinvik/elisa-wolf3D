#ifndef MIX_COMPAT_H
#define MIX_COMPAT_H

#include "sdl_compat.h"

#define MIX_CHANNELS 8

typedef struct Mix_Chunk
{
    Uint8 *abuf;
    Uint32 alen;
    Uint8 volume;
} Mix_Chunk;

int Mix_OpenAudio(int frequency, SDL_AudioFormat format, int channels, int chunksize);
int Mix_ReserveChannels(int num);
int Mix_GroupChannels(int from, int to, int tag);
int Mix_GroupAvailable(int tag);
int Mix_GroupOldest(int tag);
int Mix_SetPanning(int channel, Uint8 left, Uint8 right);
Mix_Chunk *Mix_LoadWAV_RW(SDL_IOStream *src, int freesrc);
int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops);
int Mix_HaltChannel(int channel);
void Mix_FreeChunk(Mix_Chunk *chunk);
void Mix_HookMusic(void (*mix_func)(void *udata, Uint8 *stream, int len), void *arg);
void Mix_ChannelFinished(void (*channel_finished)(int channel));
const char *Mix_GetError(void);

#endif

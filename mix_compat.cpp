#include "mix_compat.h"

#include <string.h>

typedef struct
{
    Mix_Chunk *chunk;
    Uint32 pos;
    bool active;
    Uint8 left;
    Uint8 right;
    int tag;
    Uint32 started;
} MixChannel;

static SDL_AudioStream *audioStream = NULL;
static SDL_AudioSpec outputSpec;
static MixChannel channels[MIX_CHANNELS];
static Uint32 nextStarted = 1;

static void (*finishedCallback)(int channel) = NULL;
static void (*musicCallback)(void *udata, Uint8 *stream, int len) = NULL;
static void *musicUserdata = NULL;

static Sint16 clampSample(int sample)
{
    if(sample > 32767)
        return 32767;
    if(sample < -32768)
        return -32768;
    return (Sint16)sample;
}

static void finishChannel(int channel)
{
    channels[channel].active = false;
    channels[channel].chunk = NULL;
    channels[channel].pos = 0;
    if(finishedCallback)
        finishedCallback(channel);
}

static void mixChannel(MixChannel *channel, Sint16 *dst, int frames)
{
    Mix_Chunk *chunk = channel->chunk;
    if(!chunk)
        return;

    Sint16 *src = (Sint16 *)(void *)(chunk->abuf + channel->pos);
    int availableFrames = (int)((chunk->alen - channel->pos) / 4);
    int mixFrames = frames < availableFrames ? frames : availableFrames;
    int left = channel->left;
    int right = channel->right;

    for(int i = 0; i < mixFrames; i++)
    {
        int srcLeft = src[i * 2 + 0];
        int srcRight = src[i * 2 + 1];
        dst[i * 2 + 0] = clampSample(dst[i * 2 + 0] + (srcLeft * left) / 255);
        dst[i * 2 + 1] = clampSample(dst[i * 2 + 1] + (srcRight * right) / 255);
    }

    channel->pos += (Uint32)(mixFrames * 4);
}

static void SDLCALL audioCallback(void *, SDL_AudioStream *stream, int additionalAmount, int)
{
    if(additionalAmount <= 0)
        return;

    const int frameSize = 4; // S16 stereo
    int len = ((additionalAmount + frameSize - 1) / frameSize) * frameSize;
    Uint8 *buffer = (Uint8 *)SDL_calloc(1, (size_t)len);
    if(!buffer)
        return;

    if(musicCallback)
        musicCallback(musicUserdata, buffer, len);

    Sint16 *samples = (Sint16 *)(void *)buffer;
    int frames = len / frameSize;

    for(int channel = 0; channel < MIX_CHANNELS; channel++)
    {
        if(!channels[channel].active)
            continue;

        mixChannel(&channels[channel], samples, frames);
        if(channels[channel].chunk && channels[channel].pos >= channels[channel].chunk->alen)
            finishChannel(channel);
    }

    SDL_PutAudioStreamData(stream, buffer, len);
    SDL_free(buffer);
}

int Mix_OpenAudio(int frequency, SDL_AudioFormat format, int channelsRequested, int)
{
    if(audioStream)
        return 0;

    outputSpec.freq = frequency;
    outputSpec.format = format;
    outputSpec.channels = channelsRequested;

    if(outputSpec.channels != 2 || outputSpec.format != AUDIO_S16)
    {
        SDL_SetError("mix_compat only supports signed 16-bit stereo output");
        return -1;
    }

    for(int i = 0; i < MIX_CHANNELS; i++)
    {
        channels[i].chunk = NULL;
        channels[i].pos = 0;
        channels[i].active = false;
        channels[i].left = 255;
        channels[i].right = 255;
        channels[i].tag = 0;
        channels[i].started = 0;
    }

    audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                            &outputSpec, audioCallback, NULL);
    if(!audioStream)
        return -1;

    if(!SDL_ResumeAudioStreamDevice(audioStream))
        return -1;

    return 0;
}

int Mix_ReserveChannels(int)
{
    return 0;
}

int Mix_GroupChannels(int from, int to, int tag)
{
    if(from < 0)
        from = 0;
    if(to >= MIX_CHANNELS)
        to = MIX_CHANNELS - 1;
    for(int i = from; i <= to; i++)
        channels[i].tag = tag;
    return to >= from ? (to - from + 1) : 0;
}

int Mix_GroupAvailable(int tag)
{
    for(int i = 0; i < MIX_CHANNELS; i++)
        if(channels[i].tag == tag && !channels[i].active)
            return i;
    return -1;
}

int Mix_GroupOldest(int tag)
{
    int oldest = -1;
    for(int i = 0; i < MIX_CHANNELS; i++)
    {
        if(channels[i].tag != tag || !channels[i].active)
            continue;
        if(oldest == -1 || channels[i].started < channels[oldest].started)
            oldest = i;
    }
    return oldest;
}

int Mix_SetPanning(int channel, Uint8 left, Uint8 right)
{
    if(channel < 0 || channel >= MIX_CHANNELS)
        return 0;

    if(audioStream)
        SDL_LockAudioStream(audioStream);
    channels[channel].left = left;
    channels[channel].right = right;
    if(audioStream)
        SDL_UnlockAudioStream(audioStream);

    return 1;
}

Mix_Chunk *Mix_LoadWAV_RW(SDL_IOStream *src, int freesrc)
{
    SDL_AudioSpec srcSpec;
    Uint8 *srcAudio = NULL;
    Uint32 srcLen = 0;
    if(!SDL_LoadWAV_IO(src, freesrc != 0, &srcSpec, &srcAudio, &srcLen))
        return NULL;

    Uint8 *convertedAudio = NULL;
    int convertedLen = 0;
    if(!SDL_ConvertAudioSamples(&srcSpec, srcAudio, (int)srcLen,
                                &outputSpec, &convertedAudio, &convertedLen))
    {
        SDL_free(srcAudio);
        return NULL;
    }
    SDL_free(srcAudio);

    Mix_Chunk *chunk = (Mix_Chunk *)SDL_malloc(sizeof(Mix_Chunk));
    if(!chunk)
    {
        SDL_free(convertedAudio);
        return NULL;
    }

    chunk->abuf = convertedAudio;
    chunk->alen = (Uint32)convertedLen;
    chunk->volume = 128;
    return chunk;
}

int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int)
{
    if(!chunk)
        return -1;
    if(channel < 0)
        channel = Mix_GroupAvailable(1);
    if(channel < 0 || channel >= MIX_CHANNELS)
        return -1;

    if(audioStream)
        SDL_LockAudioStream(audioStream);

    channels[channel].chunk = chunk;
    channels[channel].pos = 0;
    channels[channel].active = true;
    channels[channel].started = nextStarted++;

    if(audioStream)
        SDL_UnlockAudioStream(audioStream);

    return channel;
}

int Mix_HaltChannel(int channel)
{
    if(audioStream)
        SDL_LockAudioStream(audioStream);

    if(channel < 0)
    {
        for(int i = 0; i < MIX_CHANNELS; i++)
            if(channels[i].active)
                finishChannel(i);
    }
    else if(channel < MIX_CHANNELS && channels[channel].active)
        finishChannel(channel);

    if(audioStream)
        SDL_UnlockAudioStream(audioStream);

    return 0;
}

void Mix_FreeChunk(Mix_Chunk *chunk)
{
    if(!chunk)
        return;
    SDL_free(chunk->abuf);
    SDL_free(chunk);
}

void Mix_HookMusic(void (*mix_func)(void *udata, Uint8 *stream, int len), void *arg)
{
    if(audioStream)
        SDL_LockAudioStream(audioStream);
    musicCallback = mix_func;
    musicUserdata = arg;
    if(audioStream)
        SDL_UnlockAudioStream(audioStream);
}

void Mix_ChannelFinished(void (*channel_finished)(int channel))
{
    finishedCallback = channel_finished;
}

const char *Mix_GetError(void)
{
    return SDL_GetError();
}

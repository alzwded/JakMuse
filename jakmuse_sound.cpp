#include "jakmuse_common.h"

#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include <signal.h>

// we don't want our function overflowing
#define LIMIT(f) (signed char)(((f > 127.0f) ? 127.0f : ((f < -127.f) ? -127.f : f)))

namespace{
struct context {
    size_t i, k[5];
    SDL_AudioSpec* spec;
};
} // namespace

static void siginth(int num)
{
    printf("Exiting\n");
    SDL_CloseAudio();
    SDL_Quit();
    exit(0);
}

static void mixin(Uint8* dst, Uint8* src, int length, float scale)
{
    SDL_MixAudio(dst, src, length, SDL_MIX_MAXVOLUME);
}

static void audio_callback(void* data, Uint8* stream, int length)
{
    context* ctx = (context*)(data);
    SDL_AudioSpec spec = *((SDL_AudioSpec*)ctx->spec);
#define I() (ctx->i)
#define K(I) (ctx->k[I])
#define CHANNELS() (g_channels)
#define CONDITION(N) (CHANNELS()[N].size() > I() && CHANNELS()[N][I()].freq)
    float factor = 32.f;

    memset(stream, 0, length);

    int totl;// = spec.freq / (stuff[I()]/2);

    Uint8* buffer = (Uint8*)malloc(length);
    float scale = 1.0f;

#define FILLCHANNEL(idx) do{\
    if CONDITION(idx) {\
        totl = spec.freq / (CHANNELS()[idx][I()].freq / 2);\
        for(size_t j = 0; j < length; ++j) {\
            if(K(idx) < (float)CHANNELS()[idx][I()].fill/256.f * totl) {\
                buffer[j] = factor;\
            } else {\
                buffer[j] = 0;\
            }\
            K(idx) = (K(idx) + 1) % totl;\
        }\
        mixin(stream, buffer, length, scale);\
    }\
}while(0)

    for(size_t i = 0; i < 3; ++i) {
        FILLCHANNEL(i);
    }

#define TRIACHANNEL(idx) do{\
    if CONDITION(idx) { \
        totl = spec.freq / (CHANNELS()[idx][I()].freq / 2); \
        for(size_t j = 0; j < length; ++j) { \
            buffer[j] = (Uint8)LIMIT( (float)abs((float)(K(idx) % totl) - totl/2.f) / (float)(totl/2) * 2.f * factor - factor); \
            K(idx) = (K(idx) + 1) % totl; \
        } \
        mixin(stream, buffer, length, scale); \
    } \
}while(0)

    for(size_t i = 3; i < 5; ++i) {
        TRIACHANNEL(i);
    }

}

void play_music()
{
    SDL_AudioSpec as, spec;
    context ctx;

    SDL_Init(SDL_INIT_AUDIO);
    signal(SIGINT, &siginth);

    // set up AudioSpec
    as.freq = 44100;
    as.format = AUDIO_S16SYS;
    as.channels = 1;
    as.samples = 512;
    as.callback = &audio_callback;
    // pass a pointer with the obtained audio spec back to the callback
    ctx.spec = &spec;
    ctx.i = 0;
    memset(ctx.k, 0, 5 * sizeof(size_t));
    as.userdata = &ctx;

    // start playing music
    SDL_OpenAudio(&as, &spec);
    SDL_PauseAudio(0);

    while(1) {
        SDL_Delay(17); // arbitrary delay => note length
        // prevent the callback from firing mid-modification
        SDL_LockAudio();
        // shift to the next note
        ctx.i = (ctx.i + 1) % g_maxChannelLen;
        // resume callback
        SDL_UnlockAudio();
    }
}

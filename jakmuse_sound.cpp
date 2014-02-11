#include "jakmuse_common.h"
#include "jakmuse_sound.hpp"

#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <ctime>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include <signal.h>

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
    int totl;
    float factor = 32.f; // CAREFUL changing this
    memset(stream, 0, length);

    Uint8* buffer = (Uint8*)malloc(length);
    float scale = 1.0f; // not used

    for(size_t i = 0; i < 3; ++i) {
        FILLCHANNEL(i);
    }

    for(size_t i = 3; i < 5; ++i) {
        SAWTRICHANNEL(i);
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
        SDL_Delay(4); // arbitrary delay => note length
        // prevent the callback from firing mid-modification
        SDL_LockAudio();
        // shift to the next note
        ctx.i = (ctx.i + 1) % g_maxChannelLen;
        // resume callback
        SDL_UnlockAudio();
    }
}

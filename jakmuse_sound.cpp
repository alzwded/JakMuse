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

std::vector<short> g_wav;
size_t g_it;

#define FRAME_LEN  8500000l
#define INSTR_PER_FRAME 125000l
// why does clock_nanosleep work as intended but nanosleep yields different
//    sleep amounts on different computers?
#define SLEEP(FIN, INI) do{\
    ts.tv_sec = 0; \
    ts.tv_nsec = FRAME_LEN - (FIN.tv_nsec - INI.tv_nsec); \
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL); \
}while(0)

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

static void audio_callback(void* data, Uint8* stream, int length)
{
    context* ctx = (context*)(data);
    SDL_AudioSpec spec = *((SDL_AudioSpec*)ctx->spec);
    int realLength = std::min(length/2, (int)g_wav.size());

    if(g_it + realLength < g_wav.size()) {
        memcpy(stream, g_wav.data() + g_it, realLength * sizeof(short));
    } else {
        memcpy(stream, g_wav.data() + g_it, (g_wav.size() - g_it) * sizeof(short));
        memcpy(stream, g_wav.data(), (realLength - (g_wav.size() - g_it)) * sizeof(short));
    }
    g_it = (g_it + realLength) % g_wav.size();
}

void play_music()
{
    SDL_AudioSpec as, spec;
    context ctx;

    SDL_Init(SDL_INIT_AUDIO);
    signal(SIGINT, &siginth);

    // set up AudioSpec
    as.freq = JAKMUSE_SAMPLES_PER_SECOND;
    as.format = AUDIO_S16SYS;
    as.channels = 1;
    as.samples = JAKMUSE_BUFFER_LEN;
    as.callback = &audio_callback;
    // pass a pointer with the obtained audio spec back to the callback
    ctx.spec = &spec;
    ctx.i = 0;
    memset(ctx.k, 0, 5 * sizeof(size_t));
    as.userdata = &ctx;

    g_it = 0;

    // start playing music
    SDL_OpenAudio(&as, &spec);
    SDL_PauseAudio(0);

    while(1) {
        SDL_Event event;
        while(SDL_PollEvent(&event))
            ;
    }
}

void mix()
{
    for(size_t i = 0; i < g_maxChannelLen; ++i) {
        float sum(0.f);
        float magni = 0x7FFF;
        for(size_t j = 0; j < JAKMUSE_NUMCHANNELS; ++j) {
            if(g_channels[j].size() <= i) continue;
            printf("%zd: %d @%d\n", j, g_channels[j][i].sample, g_channels[j][i].volume);
            sum += (g_channels[j][i].sample / magni)
                * (g_channels[j][i].volume / 255.f);
            printf("    sum = %f\n", sum);
        }
        g_wav.push_back((short)(tanhf(sum) * magni));
        printf("    tanh = %d\n", g_wav[i]);
    }
}

#include "jakmuse_common.h"

#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <ctime>

#ifndef _MSC_VER
# include <SDL/SDL.h>
# include <SDL/SDL_audio.h>
#else
# include <SDL.h>
# include <SDL_audio.h>
# include <algorithm>
#endif

#include <signal.h>

std::vector<short> g_wav;
static size_t g_it;

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
        memcpy(stream + (g_wav.size() - g_it) * sizeof(short), g_wav.data(), (realLength - (g_wav.size() - g_it)) * sizeof(short));
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
        SDL_Delay(100);
    }
}

void mix()
{
    // omp-ing this yields a 10% speed increase; not much, but it's something
    g_wav.resize(g_maxChannelLen);
    #pragma omp parallel for
    for(int i = 0; i < g_maxChannelLen; ++i) {
        float sum(0.f);
        for(size_t j = 0; j < JAKMUSE_NUMCHANNELS; ++j) {
            if(g_channels[j].size() <= i) continue;
            sum += g_channels[j][i];
        }
        g_wav[i] = ((short)(tanhf(sum) * 0x7FFF));
    }
}

static void process_params(
        std::map<std::string, unsigned>& params,
        generator_t& gen,
        unsigned& scale)
{
    std::map<std::string, unsigned>::iterator found;
    // scale and lfo wave length are special...
    if((found = params.find("NPS")) != params.end()) {
        scale = found->second;
    }
    if((found = params.find("Filter")) != params.end()) {
        float filter_RC = 1.f / (2.f * 3.14159f * found->second);
        static float timestep = 1.f / JAKMUSE_SAMPLES_PER_SECOND;
        float filter_alpha = timestep / (timestep + filter_RC);

        gen.SetFilterAlpha(filter_alpha);
    }

#define process_params_CONDITION_float(KEY, VARNAME) \
    if((found = params.find(#KEY)) != params.end()) { \
        float val = (float)found->second / 255.f; \
        gen.Set##VARNAME(val); \
    }
#define process_params_CONDITION_duration(KEY, VARNAME) \
    if((found = params.find(#KEY)) != params.end()) { \
        unsigned val = (float)found->second / 4096.f * JAKMUSE_SAMPLES_PER_SECOND; \
        gen.Set##VARNAME(val); \
    }
#define process_params_CONDITION(KEY, VARNAME) \
    if((found = params.find(#KEY)) != params.end()) { \
        gen.Set##VARNAME(found->second); \
    }

    process_params_CONDITION_float(MaxVol, MaxVol);
    process_params_CONDITION(Fill, Fill);
    process_params_CONDITION_duration(A, EnvelopeA);
    process_params_CONDITION_duration(D, EnvelopeD);
    process_params_CONDITION_float(S, EnvelopeS);
    process_params_CONDITION_duration(R, EnvelopeR);
    process_params_CONDITION(LFOFreq, LfoFrequency);
    process_params_CONDITION_duration(LFOPhase, LfoPhase);
    process_params_CONDITION_float(LFODepth, LfoDepth);
#undef process_params_CONDITION
#undef process_params_CONDITION_float
}

void compile()
{
    #pragma omp parallel for schedule(dynamic, 1)
    for(int channel = 0; channel < JAKMUSE_NUMCHANNELS; ++channel) {
        auto& seqs = g_channel_sequences[channel];
        auto& gen = g_generators[channel];
        for(auto& seq : seqs) {
            auto& params = seq.params;
            auto& notes = seq.notes;

            unsigned scale(0);
            process_params(params, gen, scale);

            for(auto& note : notes) {
                unsigned numSamples =
                    JAKMUSE_SAMPLES_PER_SECOND / scale * note.length;
                gen.NewNote(note.frequency);

                for(size_t i = 0; i < numSamples; ++i) {
                    pwm_t sample = gen();
                    g_channels[channel].push_back(sample);
                }
            }
        }
    }

    // compute g_maxChannelLen
    g_maxChannelLen = 0;
    for(auto&& chan : g_channels) {
        g_maxChannelLen = std::max(g_maxChannelLen, chan.size());
    }
}

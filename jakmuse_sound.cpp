/*
Copyright (c) 2014-2015, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

static void siginth(int num)
{
    printf("Exiting\n");
    SDL_CloseAudio();
    SDL_Quit();
    exit(0);
}

static void audio_callback(void*, Uint8* stream, int length)
{
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

    SDL_Init(SDL_INIT_AUDIO);
    signal(SIGINT, &siginth);

    // set up AudioSpec
    as.freq = JAKMUSE_SAMPLES_PER_SECOND;
    as.format = AUDIO_S16SYS;
    as.channels = 1;
    as.samples = JAKMUSE_BUFFER_LEN;
    as.callback = &audio_callback;
    as.userdata = NULL;

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
        scale || (scale = 1);
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
    process_params_CONDITION_float(LFOPhase, LfoPhase);
    process_params_CONDITION_duration(Glide, GlideDuration);
    process_params_CONDITION_float(LFODepth, LfoDepth);
#undef process_params_CONDITION
#undef process_params_CONDITION_duration
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

            unsigned scale(1);
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

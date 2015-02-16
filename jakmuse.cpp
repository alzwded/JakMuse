/*
Copyright (c) 2014-2015, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "jakmuse_common.h"

#ifndef VERSION
# error "VERSION is not defined."
#endif

#define COPYRIGHT_YEAR "2014-2015"

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <string>
#include <algorithm>

size_t g_maxChannelLen = 0;
channels_t g_channels;
generators_t g_generators;

#define PRINT(WHAT, VAR) printf(#WHAT "\n"); \
for(size_t i = 0; i < JAKMUSE_NUMCHANNELS; ++i) {\
    printf("channel %ld:\n", i); \
    for(size_t j = 0; j < g_channels[i].size(); ++j) { \
        printf("%5d", g_channels[i][j].VAR); \
        if(j % 16 == 15 && j != g_channels[i].size() - 1) printf("\n"); \
    } \
    printf("\n"); \
}

static void usage(char const* argv0)
{
    printf("Usage: %s [-w file.wav|-v|-h]\n", argv0);
    exit(255);
}

int main(int argc, char* argv[])
{
    bool wav(false);

    if(argc > 1 && (argv[1][0] == '-' || argv[1][0] == '/') && argv[1][1]) {
        switch(argv[1][1]) {
        case 'v':
            printf("JakMuse v%s, Copyright %s, Vlad Mesco\n", VERSION, COPYRIGHT_YEAR);
            printf("  available under the terms of the Simplified BSD License\n");
            exit(255);
            break;
        case 'h':
        case '?':
            usage(argv[0]);
            break;
        case 'w':
            wav = true;
            break;
        default:
            usage(argv[0]);
        }
    } else if(argc > 1) {
        usage(argv[0]);
    }

    extern void init_generators();
    init_generators();

    extern void parse();
    parse();

    extern void compile();
    compile();
    // free up memory
    g_channel_sequences.clear();

    extern void mix();
    mix();
    // free up memory
    std::for_each(&g_channels[0], &g_channels[JAKMUSE_NUMCHANNELS],
            [](pwm_channel_t& chan) {
                chan.clear();
            });

    std::string filename("jakmuse.wav");
    if(wav) {
        if(argc > 2) {
            filename.assign(argv[2]);
        }

        extern bool wav_write_file(std::string const&, std::vector<short> const&, unsigned);
        extern std::vector<short> g_wav;
        auto hr = wav_write_file(filename, g_wav, JAKMUSE_SAMPLES_PER_SECOND);

        if(hr) {
            printf("Wrote %s\n", filename.c_str());
            exit(0);
        } else {
            printf("Could not write %s\n", filename.c_str());
            exit(hr);
        }
    } else {
        extern void play_music();
        play_music();
    }

    return 0;
}


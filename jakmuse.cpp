#include "jakmuse_common.h"

#ifndef VERSION
# error "VERSION is not defined."
#endif

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <string>

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

int main(int argc, char* argv[])
{
    if(argc > 1 && strcmp(argv[1], "-v") == 0) {
        printf("JakMuse v%s, Copyright Vlad Mesco 2014\n", VERSION);
        printf("  available under the terms of the Simplified BSD License\n");
        exit(255);
    }

    extern void init_generators();
    init_generators();

    extern void parse();
    parse();

    extern void mix();
    mix();

    std::string filename("jakmuse.wav");
    if(argc > 1 && strcmp(argv[1], "-w") == 0) {
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


#include "jakmuse_common.h"

#include <cstdio>
#include <cassert>

size_t g_maxChannelLen = 0;
channels_t g_channels;

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
    extern void parse();
    parse();

    PRINT(fills, fill);
    PRINT(freqs, freq);

    extern void play_music();
    play_music();

    return 0;
}


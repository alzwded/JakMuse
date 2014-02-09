#include "jakmuse_common.h"

#include <cstdio>
#include <cassert>

#include <vector>

size_t maxChannelLen = 0;
std::vector<pwm> channels[5];

#define PRINT(WHAT, VAR) printf(#WHAT "\n"); \
for(size_t i = 0; i < 5; ++i) {\
    printf("channel %ld:\n", i); \
    for(size_t j = 0; j < channels[i].size(); ++j) { \
        printf("%5d", channels[i][j].VAR); \
        if(j % 16 == 15 && j != channels[i].size() - 1) printf("\n"); \
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


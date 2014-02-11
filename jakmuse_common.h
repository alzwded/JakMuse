#ifndef JAKMUSE_COMMON_H
#define JAKMUSE_COMMON_H

#include <vector>
#include <cstddef>

#define JAKMUSE_NUMCHANNELS 5

struct pwm {
    unsigned short freq;
    unsigned short fill;
};

extern size_t g_maxChannelLen;
typedef std::vector<pwm> pwm_channel_t;
typedef pwm_channel_t channels_t[JAKMUSE_NUMCHANNELS];
extern channels_t g_channels;

#endif

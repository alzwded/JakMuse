#ifndef JAKMUSE_COMMON_H
#define JAKMUSE_COMMON_H

#include <vector>
#include <cstddef>

struct pwm {
    unsigned short freq;
    unsigned short fill;
};

extern size_t g_maxChannelLen;
typedef std::vector<pwm> pwm_channel_t;
extern pwm_channel_t g_channels[5];

#endif

#ifndef JAKMUSE_COMMON_H
#define JAKMUSE_COMMON_H

#include <vector>
#include <cstddef>

#define JAKMUSE_NUMCHANNELS 7

#define JAKMUSE_SAMPLES_PER_SECOND 11025
//#define JAKMUSE_SAMPLES_PER_SECOND 44100
#define JAKMUSE_BUFFER_LEN 4096

typedef struct {
    short sample;
    unsigned short volume;
} pwm_t;

extern size_t g_maxChannelLen;
typedef std::vector<pwm_t> pwm_channel_t;
typedef pwm_channel_t channels_t[JAKMUSE_NUMCHANNELS];
extern channels_t g_channels;
typedef short (*generator_t)(unsigned short freq, unsigned Ns, unsigned fill, float alpha);
typedef std::vector<generator_t> generators_t;
extern generators_t g_generators;

#define GENERATOR_RESET_PARAMS(ALPHA) 0, 1, 1, ALPHA

#endif

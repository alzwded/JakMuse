#include <cmath>
#include "jakmuse_common.h"

static short _square(unsigned short freq, unsigned Ns, unsigned fill)
{
    static unsigned k = 0;
    static short values[2] = { 127, -128 };

    unsigned lk = k++ % Ns;
    return values[lk < Ns * fill / 256];
}

#include <cstdio>
static short _triangle(unsigned short freq, unsigned Ns, unsigned fill)
{
    static unsigned k = 0;

    unsigned lk = k++ % Ns;

    float val_1 = (lk < Ns * fill / 256)
            ? (float)lk * 256 / (Ns * fill) * 2.f - 1.f
            : (float)(Ns - lk) / (Ns - Ns * fill / 256) * 2.f - 1.f
            ;
    printf("%u %f|Ns = %u fill = %u freq = %u\n", lk, val_1, Ns, fill, freq);
    return (short)(127.f * val_1);
}

static short _sine(unsigned short freq, unsigned Ns, unsigned fill)
{
    static unsigned k = 0;

    unsigned lk = k % Ns;
    // use cos in order to keep triangle and sine in phase
    // for fill = 128
    float val_1 = cosf(1.f / Ns * 2.f * 3.14159f * lk
            + 2.f * 3.14159f * fill / 256.f);

    return (short)(127.f * val_1);
}

static short _noise(unsigned short freq, unsigned Ns, unsigned fill)
{
    typedef struct {
        unsigned short reg, poly;
    } _reg_t;

    static _reg_t regs[2] = {
        { 0xA001, 0x8255 },
        { 0xA001, 0xA801 },
    };

    static unsigned k = 0;

    _reg_t* myreg = &regs[freq < 440];
    unsigned lf = k++ % fill;

    if(lf == 0) {
        myreg->reg = (myreg->reg >> 1) ^ ((myreg->reg & 0x1) * myreg->poly);
    }

    return (short)myreg->reg;
}


void init_generators()
{
    g_generators.push_back(_square);
    g_generators.push_back(_square);
    g_generators.push_back(_triangle);
    g_generators.push_back(_triangle);
    g_generators.push_back(_noise);
#if 0
    g_generators.push_back(_sine);
    g_generators.push_back(_sine);
#endif
}

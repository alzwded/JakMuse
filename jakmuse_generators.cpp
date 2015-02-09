#include <cmath>
#include "jakmuse_common.h"

#define APPLY_RC(X, A, F) do{\
    static short APPLY_RC_X_1 = 0; \
    if(F == 0) X = 0; \
    short nv = A * X + (1 - A) * APPLY_RC_X_1; \
    APPLY_RC_X_1 = nv; \
    X = nv; \
}while(0)

// TODO generators return float because they go directly into the mixer

static short _square(unsigned short freq, unsigned Ns, unsigned fill, float alpha)
{
    static unsigned k = 0;
    static short values[2] = { 0x7FFF, -0x7FFF };

    unsigned lk = k++ % Ns;
    short ret = values[lk < Ns * fill / 256];
    APPLY_RC(ret, alpha, freq);
    return ret;
}

static short _triangle(unsigned short freq, unsigned Ns, unsigned fill, float alpha)
{
    static unsigned k = 0;

    unsigned lk = k++ % Ns;

    float val_1 = (lk < Ns * fill / 256)
            ? (float)lk * 256 / (Ns * fill) * 2.f - 1.f
            : (float)(Ns - lk) / (Ns - Ns * fill / 256) * 2.f - 1.f
            ;
    float magni = 0x7FFF;
    short ret = (short)(magni * val_1);
    APPLY_RC(ret, alpha, freq);
    return ret;
}

static short _sine(unsigned short freq, unsigned Ns, unsigned fill, float alpha)
{
    static unsigned k = 0;

    unsigned lk = k++ % Ns;
    // use cos in order to keep triangle and sine in phase
    // for fill = 128
    float val_1 = cosf(1.f / Ns * 2.f * 3.14159f * lk
            + 2.f * 3.14159f * fill / 256.f);

    float magni = 0x7FFF;
    short ret = (short)(magni * val_1);
    APPLY_RC(ret, alpha, freq);
    return ret;
}

static short _noise(unsigned short freq, unsigned Ns, unsigned fill, float alpha)
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

    short ret = (short)myreg->reg;
    APPLY_RC(ret, alpha, freq);
    return ret;
}


void init_generators()
{
    g_generators.push_back(_square);
    g_generators.push_back(_square);
    g_generators.push_back(_triangle);
    g_generators.push_back(_triangle);
    g_generators.push_back(_noise);
    g_generators.push_back(_sine);
    g_generators.push_back(_sine);
}

#include <cmath>
#include "jakmuse_common.h"

// in semitones
#define LFO_TUNING_MAX 1.f

static float _square(unsigned k, noise_reg_t noise_regs[], unsigned short Ns, unsigned short fill)
{
    if(Ns == 0) return 0.f;

    static float values[2] = { 1.f, -1.f };

    unsigned lk = k++ % Ns;
    return values[lk < Ns * fill / 256];
}

static float _triangle(unsigned k, noise_reg_t noise_regs[], unsigned short Ns, unsigned short fill)
{
    if(Ns == 0) return 0.f;

    unsigned lk = k++ % Ns;

    float val_1 = (lk < Ns * fill / 256)
            ? (float)lk * 256 / (Ns * fill) * 2.f - 1.f
            : (float)(Ns - lk) / (Ns - Ns * fill / 256) * 2.f - 1.f
            ;
    return val_1;
}

static float _sine(unsigned k, noise_reg_t noise_regs[], unsigned short Ns, unsigned short fill)
{
    if(Ns == 0) return 0.f;
    
    unsigned lk = k++ % Ns;
    // use cos in order to keep triangle and sine in phase
    // for fill = 128
    float val_1 = cosf(1.f / Ns * 2.f * 3.14159f * lk
            + 2.f * 3.14159f * fill / 256.f);

    return val_1;
}

static float _noise(unsigned k, noise_reg_t noise_regs[], unsigned short Ns, unsigned short fill)
{
    if(Ns == 0) return 0.f;
    
    noise_reg_t* myreg = &noise_regs[Ns > JAKMUSE_SAMPLES_PER_SECOND / 440];
    unsigned lf = k++ % fill;

    if(lf == 0) {
        // this is one reason why calls to generate can't be parallelized
        myreg->reg = (myreg->reg >> 1) ^ ((myreg->reg & 0x1) * myreg->poly);
    }

    return (float)myreg->reg / (float)0x7FFF;
}


void init_generators()
{
    g_generators.push_back(Generator(_square));
    g_generators.push_back(Generator(_square));
    g_generators.push_back(Generator(_triangle));
    g_generators.push_back(Generator(_triangle));
    g_generators.push_back(Generator(_noise));
    g_generators.push_back(Generator(_sine));
    g_generators.push_back(Generator(_sine));
}

float Generator::operator()()
{
    // grab the 'frequency' (wavelength, actually)
    unsigned freq = state_.pub.def.freq;
    unsigned last_freq = state_.priv.last_freq;
    unsigned Ns = (freq) ? JAKMUSE_SAMPLES_PER_SECOND / freq : 0;
    unsigned last_Ns = (last_freq) ? JAKMUSE_SAMPLES_PER_SECOND / last_freq : 0;

    // compute base sample
    float base = fn_(state_.priv.k, state_.priv.noise_regs, Ns, state_.pub.def.fill);
    float prev = fn_(state_.priv.k, state_.priv.noise_regs, last_Ns, state_.pub.def.fill);

    // TODO glide... maybe

    // apply volume
#define ADSR_COUNTER (state_.priv.adsr_counter)
    if(ADSR_COUNTER < state_.pub.volume.A) {
        base = (float)ADSR_COUNTER/state_.pub.volume.A
            * state_.pub.volume.maxvol
            * base;
        ADSR_COUNTER++;
    } else if(ADSR_COUNTER - state_.pub.volume.A < state_.pub.volume.D) {
        base = (1.f
                - (float)(ADSR_COUNTER - state_.pub.volume.A)
                / state_.pub.volume.D)
            * state_.pub.volume.maxvol
            * base;
        ADSR_COUNTER++;
    } else {
        base = state_.pub.volume.maxvol * state_.pub.volume.S * base;
    }

    // don't check the modulated Ns here; Ns == 0 means no note is input
    if(state_.pub.def.freq == 0
            && ADSR_COUNTER - state_.pub.volume.A - state_.pub.volume.D
            < state_.pub.volume.R)
    {
        base = (1.f - (float)(ADSR_COUNTER
                        - state_.pub.volume.A - state_.pub.volume.D)
                    / state_.pub.volume.R)
            * state_.pub.volume.maxvol
            * state_.pub.volume.S
            * prev;
        ADSR_COUNTER++;
    }
#undef ADSR_COUNTER

    // apply amplitude lfo
    float lfo_sample = cosf(state_.priv.k
            * (float)state_.pub.lfo.freq / JAKMUSE_SAMPLES_PER_SECOND
            * 2.f * 3.14159f
            + (float)state_.pub.lfo.phase);
    base = state_.pub.lfo.depth * lfo_sample * base
        + (1.f - state_.pub.lfo.depth) * base;

    // apply RC filter
    float note = state_.pub.def.alpha * base +
        (1.f - state_.pub.def.alpha) * state_.priv.rc_reg;
    state_.priv.rc_reg = note;

    // increment step
    state_.priv.k++;

    return note;
}

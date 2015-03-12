/*
Copyright (c) 2014-2015, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <cmath>
#include "jakmuse_common.h"

#include <algorithm>

#ifdef _MSC_VER
# include <float.h>
#endif

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
    float zeroPosition = (float)fill / 255.f;
    unsigned zp = Ns * zeroPosition;
#if 1
    float val_1 = sinf(3.14159f * 
            ((lk < zp)
            ? ((float)lk / zp)            
            : ( (float)(lk - zp)
                    / (float)(Ns - zp)
                + 1.f))
            );
#else
    /* All right, let me explain...

       There's this really weird thing that's happening on a particular
       intel machine on windows where both sin and sinf start going
       haywire and returning negative infinity like crazy.

       I couldn't figure out for the life of me what the problem is,
       but calling it 2 times in a row with _clearfp in between seems
       to sort-of end up producing, you know, normal results. At least
       for my test.

       Another thing, it only starts happening more-or-less at random
       based on other things that happen beforehand. I don't know...

       If the negative infinity problem pops up again I'll probably spend
       more time figuring out what the devil's happening.
    */
    volatile float val_2 =
        ((lk < zp)
        ? ((float)lk / zp)
        : ( (float)(lk - zp)
                / (Ns - zp)
            + 1.f));
    ;
    _clearfp();
    volatile float val_1 = sinf(val_2);
    _clearfp();
    val_1 = sinf(val_2);
    //_clearfp();
    //val_1 = sinf(val_2);
#endif

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

    if(state_.pub.glide.Ns
            && freq
            && last_freq)
    {
        if(state_.priv.gl_counter < state_.priv.gl_NsPerPeriod) {
            state_.priv.gl_counter++;
        } else if(state_.priv.k % state_.priv.Ts[state_.priv.gl_idx] == 0) {
            state_.priv.gl_counter = state_.priv.gl_nextStartAt;
            state_.priv.gl_nextStartAt = 1;
            state_.priv.k = 0;
            state_.priv.gl_idx = 
                (1.03f * state_.priv.gl_passed + state_.priv.gl_NsPerPeriod)
                / state_.priv.gl_NsPerPeriod
                - 1;
            state_.priv.gl_idx = std::min(state_.priv.gl_idx, state_.priv.Ts.size() - 1);
        } else {
            state_.priv.gl_nextStartAt++;
        }
        state_.priv.gl_counter++;
        state_.priv.gl_passed++;
        Ns = state_.priv.Ts[state_.priv.gl_idx];
    }

    // compute base sample
    float base = fn_(state_.priv.k, state_.priv.noise_regs, Ns, state_.pub.def.fill);
    float prev = fn_(state_.priv.k, state_.priv.noise_regs, last_Ns, state_.pub.def.fill);

    // apply volume
#define ADSR_COUNTER (state_.priv.adsr_counter)
    if(ADSR_COUNTER < state_.pub.volume.A) {
        base = (float)ADSR_COUNTER/state_.pub.volume.A
            * state_.pub.volume.maxvol
            * base;
        ADSR_COUNTER++;
    } else if(ADSR_COUNTER - state_.pub.volume.A < state_.pub.volume.D) {
        float regl = (1.f
                - (float)(ADSR_COUNTER - state_.pub.volume.A)
                / state_.pub.volume.D);
        base = 
            (regl * (1.f- state_.pub.volume.S) + (state_.pub.volume.S))
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
    unsigned lfo_Ns = (state_.pub.lfo.freq)
        ? JAKMUSE_SAMPLES_PER_SECOND / state_.pub.lfo.freq
        : 1
        ;
    unsigned lfo_lk = state_.priv.lfo_k++ % lfo_Ns;
    unsigned lfo_zp = state_.pub.lfo.phase * lfo_Ns;
    float lfo_sample = cosf(3.14159f *
            ((lfo_lk < lfo_zp)
             ? ((float)lfo_lk / lfo_zp)
             : ( (float)(lfo_lk - lfo_zp)
                     / (lfo_Ns - lfo_zp)
                 + 1.f))
            );
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

unsigned Generator::NewNote(unsigned frequency)
{
    state_.priv.last_freq = state_.pub.def.freq;
    state_.pub.def.freq = frequency;
    if(frequency) state_.priv.adsr_counter = 0;

    if(state_.pub.glide.Ns
            && state_.priv.last_freq
            && state_.pub.def.freq)
    {
        unsigned T0 = JAKMUSE_SAMPLES_PER_SECOND / state_.priv.last_freq;
        unsigned TN = JAKMUSE_SAMPLES_PER_SECOND / state_.pub.def.freq;
        // cast it to long because cl complains it doesn't know which
        //     abs to call
        state_.priv.gl_NsPerPeriod = state_.pub.glide.Ns / abs((long)(T0 - TN + 1));
        state_.priv.gl_idx = 0;
        state_.priv.gl_counter = state_.priv.gl_NsPerPeriod;
        state_.priv.Ts.clear();
        state_.priv.gl_passed = 0;
        for(unsigned T = T0; T != TN; T += (T0 < TN) - (T0 >= TN)) {
            state_.priv.Ts.push_back(T);
        }
        state_.priv.Ts.push_back(TN);
    }

    return state_.pub.def.NPS;
}

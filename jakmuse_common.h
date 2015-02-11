#ifndef JAKMUSE_COMMON_H
#define JAKMUSE_COMMON_H

#include <vector>
#include <cstddef>

#define JAKMUSE_NUMCHANNELS 7

#define JAKMUSE_SAMPLES_PER_SECOND 11025
//#define JAKMUSE_SAMPLES_PER_SECOND 44100
#define JAKMUSE_BUFFER_LEN 4096

typedef float pwm_t;

extern size_t g_maxChannelLen;
typedef std::vector<pwm_t> pwm_channel_t;
typedef pwm_channel_t channels_t[JAKMUSE_NUMCHANNELS];
extern channels_t g_channels;

typedef struct {
    unsigned short reg, poly;
} noise_reg_t;

typedef struct {
    struct {
        unsigned Ns, fill;
        float alpha;
    } def;
    struct {
        float maxvol;
        float S;
        unsigned A, D, R;
    } volume;
    struct {
        unsigned Ns, phase;
        float depth;
        float freq_modulation_depth;
    } lfo;
} generator_public_state_t;

typedef float (*generator_fn)(unsigned k, noise_reg_t noise_regs[], unsigned short Ns, unsigned short fill);


typedef struct {
    struct {
        unsigned k;
        noise_reg_t noise_regs[2];
        unsigned last_Ns;
        float rc_reg;
        unsigned adsr_counter;
    } priv;
    generator_public_state_t pub;
} generator_state_t;

extern void init_generators();

typedef class Generator
{
    generator_state_t state_;
    generator_fn fn_;

    Generator(generator_fn fn)
        : fn_(fn)
    {
        static generator_state_t default_ = {
            // private
            {
                // internal counter
                0,
                // noise registers
                { { 0xA001, 0x8255 }, { 0xA001, 0xA801 } },
                // last/previous note frequency
                0,
                // filter's previous value register
                0.f,
                // adsr counter
                0,
            },
            // public
            {
                // note definition
                {
                    // note freq, fill
                    0, 128,
                    // filter's alpha parameter
                    1.1f,
                },
                // volume
                {
                    // maxvol
                    0.5f,
                    // sustain level
                    1.f,
                    // attack, decay, release
                    0, 0, 0,
                },
                // lfo
                {
                    // freq, phase
                    0, 0,
                    // depth
                    0.f,
                    // frequency modulation depth,
                    0.f,
                },
            },
        };
        state_ = default_;
    }

    friend void init_generators();
public:
    Generator(Generator const&) = default;
    Generator& operator=(Generator const&) = default;
#ifndef _MSC_VER
    Generator(Generator &&) = default;
    Generator& operator=(Generator &&) = default;
#endif


    void SetWaveLength(unsigned Ns) { state_.pub.def.Ns = Ns; }
    void SetFill(unsigned fill) { state_.pub.def.fill = fill; }
    void SetFilterAlpha(float alpha) { state_.pub.def.alpha = alpha; }
    void SetMaxVol(float vol) { state_.pub.volume.maxvol = vol; }
    void SetEnvelopeA(unsigned A) { state_.pub.volume.A = A; }
    void SetEnvelopeD(unsigned D) { state_.pub.volume.D = D; }
    void SetEnvelopeS(float S) { state_.pub.volume.S = S; }
    void SetEnvelopeR(unsigned R) { state_.pub.volume.R = R; }
    void SetLfoWaveLength(unsigned Ns) { state_.pub.lfo.Ns = Ns; }
    void SetLfoPhase(unsigned phase) { state_.pub.lfo.phase = phase; }
    void SetLfoDepth(float depth) { state_.pub.lfo.depth = depth; }
    void SetLfoFrequencyModulationDepth(float depth) { state_.pub.lfo.freq_modulation_depth = depth; }

    void NewNote(unsigned wavelength)
    {
        state_.priv.last_Ns = state_.pub.def.Ns;
        state_.pub.def.Ns = wavelength;
        state_.priv.adsr_counter = 0;
    }

    float operator()();
} generator_t;

typedef std::vector<generator_t> generators_t;
extern generators_t g_generators;

#endif

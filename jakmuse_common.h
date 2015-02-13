#ifndef JAKMUSE_COMMON_H
#define JAKMUSE_COMMON_H

#include <vector>
#include <map>
#include <cstddef>

#define JAKMUSE_NUMCHANNELS 7

#define JAKMUSE_SAMPLES_PER_SECOND 44100
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
        unsigned freq, fill;
        float alpha;
    } def;
    struct {
        float maxvol;
        float S;
        unsigned A, D, R;
    } volume;
    struct {
        unsigned freq;
        float phase;
        float depth;
    } lfo;
    struct {
        unsigned Ns;
    } glide;
} generator_public_state_t;

typedef float (*generator_fn)(unsigned k, noise_reg_t noise_regs[], unsigned short Ns, unsigned short fill);


typedef struct {
    struct {
        unsigned k;
        noise_reg_t noise_regs[2];
        unsigned last_freq;
        float rc_reg;
        unsigned adsr_counter;
        unsigned gl_NsPerPeriod;
        unsigned gl_idx;
        unsigned gl_nextStartAt;
        unsigned gl_counter;
        unsigned gl_passed;
        //
        std::vector<unsigned> Ts;
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
                // gl_NsPerPeriod,
                0,
                // gl_idx,
                0,
                // gl_nextStartAt,
                0,
                // gl_counter
                0,
                // gl_passed
                0,
                // auto
                //.......
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
                    // freq
                    0,
                    // phase
                    0.5f,
                    // depth
                    0.f,
                },
                // glide
                {
                    0,
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


    void SetFrequency(unsigned freq) { state_.pub.def.freq = freq; }
    void SetFill(unsigned fill) { state_.pub.def.fill = fill; }
    void SetFilterAlpha(float alpha) { state_.pub.def.alpha = alpha; }
    void SetMaxVol(float vol) { state_.pub.volume.maxvol = vol; }
    void SetEnvelopeA(unsigned A) { state_.pub.volume.A = A; }
    void SetEnvelopeD(unsigned D) { state_.pub.volume.D = D; }
    void SetEnvelopeS(float S) { state_.pub.volume.S = S; }
    void SetEnvelopeR(unsigned R) { state_.pub.volume.R = R; }
    void SetLfoFrequency(unsigned freq) { state_.pub.lfo.freq = freq; }
    void SetLfoPhase(float phase) { state_.pub.lfo.phase = phase; }
    void SetLfoDepth(float depth) { state_.pub.lfo.depth = depth; }
    void SetGlideDuration(unsigned numSamples) { state_.pub.glide.Ns = numSamples; }

    void NewNote(unsigned frequency);

    float operator()();
} generator_t;

typedef std::vector<generator_t> generators_t;
extern generators_t g_generators;

// TODO optimize moves later; right now get omp working
typedef std::map<std::string, unsigned> params_map_t;
typedef struct {
    unsigned length;
    unsigned frequency;
} note_t;
typedef struct {
    params_map_t params;
    std::vector<note_t> notes;
} sequence_t;
typedef std::vector<sequence_t> sequences_t;
typedef std::vector<sequences_t> channel_sequences_t;
extern channel_sequences_t g_channel_sequences;

#endif

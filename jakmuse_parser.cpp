#include "jakmuse_common.h"

#ifdef _MSC_VER
# include <algorithm>
#endif

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cmath>

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <utility>

static int toknum = 0;
static std::string token("");

#define ENSURE(a) do{ \
    if(!(a)) cancel(#a); \
}while(0)

#define TOKEN(a) do{ \
    std::string STR; \
    do { \
        std::cin >> STR; \
        if(STR[0] == '#') { \
            while(!std::cin.eof() && std::cin.get() != '\n'); \
            while(!std::cin.eof() && std::cin.peek() == '\n') std::cin.get(); \
        } \
    } while(!std::cin.eof() && STR[0] == '#'); \
    std::stringstream S1; \
    S1 << STR; \
    S1 >> a; \
    std::stringstream S; \
    ++toknum; \
    token.assign(STR); \
}while(0)

static void cancel(const char* assertion)
{
    fprintf(stderr, "Syntax error near token %d \"%s\"\n", toknum, token.c_str());
    fprintf(stderr, "Failed assertion '%s'\n", assertion);
    return abort();
}

static bool isnote(char c)
{
    static char notes[] = "CDEFGAB";
    return strchr(notes, c) != NULL;
}

static signed translate_note(char name)
{
    switch(name) {
    case 'C': return 0;
    case 'D': return 2;
    case 'E': return 4;
    case 'F': return 5;
    case 'G': return 7;
    case 'A': return 9;
    case 'B': return 11;
    default: ENSURE(isnote(name));
    }

    // no warning
    return 0;
}

static unsigned short get_frequency(char name, signed scale, char accidental)
{
    signed Ascale = 4;
    signed A = 9;
    signed note = translate_note(name);

    signed step = (scale - Ascale) * 12 + note - A;

    switch(accidental) {
    case '#':
        ++step;
        break;
    case 'b':
        --step;
        break;
    default:
        break;
    }

    return (unsigned short)(440.0f * std::pow(2.0f, (float)step / 12.0f));
}

static void process_params(
        std::map<std::string, unsigned>& params,
        generator_t& gen,
        unsigned& scale)
{
    std::map<std::string, unsigned>::iterator found;
    // scale and lfo wave length are special...
    if((found = params.find("NPS")) != params.end()) {
        scale = found->second;
    }
    if((found = params.find("Filter")) != params.end()) {
        float filter_RC = 1.f / (2.f * 3.14159f * found->second);
        static float timestep = 1.f / JAKMUSE_SAMPLES_PER_SECOND;
        float filter_alpha = timestep / (timestep + filter_RC);

        gen.SetFilterAlpha(filter_alpha);
    }

#define process_params_CONDITION_float(KEY, VARNAME) \
    if((found = params.find(#KEY)) != params.end()) { \
        float val = (float)found->second / 255.f; \
        gen.Set##VARNAME(val); \
    }
#define process_params_CONDITION_duration(KEY, VARNAME) \
    if((found = params.find(#KEY)) != params.end()) { \
        unsigned val = (float)found->second / 4096.f * JAKMUSE_SAMPLES_PER_SECOND; \
        gen.Set##VARNAME(val); \
    }
#define process_params_CONDITION(KEY, VARNAME) \
    if((found = params.find(#KEY)) != params.end()) { \
        gen.Set##VARNAME(found->second); \
    }

    process_params_CONDITION_float(MaxVol, MaxVol);
    process_params_CONDITION(Fill, Fill);
    process_params_CONDITION_duration(A, EnvelopeA);
    process_params_CONDITION_duration(D, EnvelopeD);
    process_params_CONDITION_float(S, EnvelopeS);
    process_params_CONDITION_duration(R, EnvelopeR);
    process_params_CONDITION(LFOFreq, LfoFrequency);
    process_params_CONDITION_duration(LFOPhase, LfoPhase);
    process_params_CONDITION_float(LFODepth, LfoDepth);
#undef process_params_CONDITION
#undef process_params_CONDITION_float
}

void parse()
{
    // CHANNEL '{' param_list '}' note_list ';' ;
    while(!std::cin.eof()) {
        // get channel
        unsigned channel(255);
        TOKEN(channel);
        if(!std::cin.good() || std::cin.eof()) break;
        ENSURE(channel < JAKMUSE_NUMCHANNELS);

        // read the '{'
        std::string scratch;
        TOKEN(scratch);
        ENSURE(scratch.compare("{") == 0);
        std::map<std::string, unsigned> params;

        do {
            TOKEN(scratch);
            if(scratch.compare("}") == 0) break;
            unsigned val;
            TOKEN(val);
            ENSURE(token.compare("}") != 0);
            params.insert(std::make_pair(scratch, val));
        } while(1);

        // configure the generator
        Generator& gen = g_generators[channel];
        unsigned scale(0);
        process_params(params, gen, scale);

        // start reading notes
        std::string s;
        do {
            TOKEN(s);
            if(s.empty() || s.compare(";") == 0) break;

            size_t i = 0;
            size_t const ns = s.size();

            unsigned length = 0;
            do {
                if(i >= ns || !isdigit(s[i])) break;
                length *= 10;
                length += s[i++] - '0';
            } while(1);
            ENSURE(length > 0);
            ENSURE(i < ns);

            char note = s[i++];
            unsigned short frequency(0);

            if(isnote(note)) {
                ENSURE(i < ns);
                char accidental = '\0';
                if(s[i] == '#' || s[i] == 'b') {
                    accidental = s[i++];
                }
                ENSURE(i < ns);

                signed offset = 0;
                do {
                    if(i >= ns || !isdigit(s[i])) break;
                    offset *= 10;
                    offset += s[i++] - '0';
                } while(1);
                ENSURE(i >= ns);

                frequency = get_frequency(note, offset, accidental);
            }

            unsigned numSamples =
                JAKMUSE_SAMPLES_PER_SECOND / scale * length;

            gen.NewNote(frequency);

            for(size_t i = 0; i < numSamples; ++i) {
                pwm_t sample = g_generators[channel]();
                g_channels[channel].push_back(sample);
            }

            g_maxChannelLen = std::max(g_maxChannelLen, g_channels[channel].size());
        } while(1);
        ENSURE(std::cin.good());
    }
}

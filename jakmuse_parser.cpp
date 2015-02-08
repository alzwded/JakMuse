#include "jakmuse_common.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cmath>

#include <iostream>
#include <string>
#include <sstream>

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

void parse()
{
    while(!std::cin.eof()) {
        // get channel
        unsigned channel(255);
        TOKEN(channel);
        if(!std::cin.good() || std::cin.eof()) break;
        ENSURE(channel < JAKMUSE_NUMCHANNELS);
        // get fill factor
        unsigned fill(128);
        TOKEN(fill);
        // get scale factor
        unsigned scale(1);
        TOKEN(scale);
        ENSURE(scale > 0);
        // get volume
        unsigned volume(128);
        TOKEN(volume);
        ENSURE(volume < 256);
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

            printf("%u ns for this note;\n", numSamples);

            for(size_t i = 0; i < numSamples; ++i) {
                short sample(0);
                sample = g_generators[channel](
                            frequency,
                            JAKMUSE_SAMPLES_PER_SECOND / frequency,
                            fill);
                pwm_t el = { sample, volume };
                g_channels[channel].push_back(el);
            }   

            g_maxChannelLen = std::max(g_maxChannelLen, g_channels[channel].size());
        } while(1);
        ENSURE(std::cin.good());
    }
}

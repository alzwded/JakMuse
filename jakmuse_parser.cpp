/*
Copyright (c) 2014-2015, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

channel_sequences_t g_channel_sequences;

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

void parse()
{
    g_channel_sequences.resize(JAKMUSE_NUMCHANNELS);
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
        //Generator& gen = g_generators[channel];
        sequence_t sequence = { params };

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

            note_t s_note = { length, frequency };
            sequence.notes.push_back(s_note);
        } while(1);
        g_channel_sequences[channel].push_back(sequence);
        ENSURE(std::cin.good());
    }
}

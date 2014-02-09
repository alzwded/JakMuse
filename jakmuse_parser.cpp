#include "jakmuse_common.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>

#include <iostream>
#include <string>

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
    default: abort();
    }
}

static unsigned short get_offset(char name, signed scale, char accidental)
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

static bool isnote(char c)
{
    static char notes[] = "CDEFGAB";
    return strchr(notes, c) != NULL;
}

void parse()
{
    while(!std::cin.eof()) {
        // get channel
        unsigned channel(255);
        std::cin >> channel;
        if(!std::cin.good()) break;
        assert(channel < 5);
        // get fill factor
        unsigned fill(128);
        std::cin >> fill;
        // get scale factor
        unsigned scale(1);
        std::cin >> scale;
        assert(scale);
        // start reading notes
        std::string s;
        do {
            std::cin >> s;
            if(s.empty() || s.compare(";") == 0) break;

            size_t i = 0;
            size_t const ns = s.size();

            int length = 0;
            do {
                if(i >= ns || !isdigit(s[i])) break;
                length *= 10;
                length += s[i++] - '0';
            } while(1);
            assert(length);
            assert(i < ns);

            char note = s[i++];
            unsigned short frequency(0);

            if(isnote(note)) {
                assert(i < ns);
                char accidental = '\0';
                if(s[i] == '#' || s[i] == 'b') {
                    accidental = s[i++];
                }
                assert(i < ns);

                signed offset = 0;
                do {
                    if(i >= ns || !isdigit(s[i])) break;
                    offset *= 10;
                    offset += s[i++] - '0';
                } while(1);
                assert(i >= ns);

                frequency = get_offset(note, offset, accidental);
            }

            for(size_t i = 0; i < length * scale; ++i) {
                pwm el = { frequency, fill };
                g_channels[channel].push_back(el);
            }   

            g_maxChannelLen = std::max(g_maxChannelLen, g_channels[channel].size());
        } while(1);
    }
}

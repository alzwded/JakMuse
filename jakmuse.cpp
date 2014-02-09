#include <cstring>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_mixer.h>

#include <signal.h>

// we don't want our function overflowing
#define LIMIT(f) (signed char)(((f > 127.0f) ? 127.0f : ((f < -127.f) ? -127.f : f)))

struct context {
    size_t i, k[5];
    SDL_AudioSpec* spec;
};

size_t maxChannelLen = 0;
struct pwm {
    unsigned short freq;
    unsigned short fill;
//    pwm() : freq(0), fill(128) {}
//    pwm(unsigned short f, unsigned short fi)
//        : freq(f), fill(fi)
//    {}
};
std::vector<pwm> channels[5];

signed translate_note(char name)
{
    switch(name) {
    case 'C': return 0;
    case 'D': return 2;
    case 'E': return 4;
    case 'F': return 5;
    case 'G': return 7;
    case 'A': return 9;
    case 'B': return 11;
    }
}

unsigned short get_offset(char name, signed offset, char accidental)
{
    signed Co = 4;
    signed C = 9;
    signed note = translate_note(name);

    signed step = (offset - Co) * 12 + note - C;

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

bool isnote(char c)
{
    static char notes[] = "CDEFGAB";
    return strchr(notes, c) != NULL;
}

void siginth(int num)
{
    printf("Exiting\n");
    SDL_CloseAudio();
    SDL_Quit();
    exit(0);
}

void mixin(Uint8* dst, Uint8* src, int length, float scale)
{
    SDL_MixAudio(dst, src, length, SDL_MIX_MAXVOLUME);
}

void audio_callback(void* data, Uint8* stream, int length)
{
    context* ctx = (context*)(data);
    SDL_AudioSpec spec = *((SDL_AudioSpec*)ctx->spec);
#define I() (ctx->i)
#define K(I) (ctx->k[I])
#define CHANNELS() (channels)
#define CONDITION(N) (CHANNELS()[N].size() > I() && CHANNELS()[N][I()].freq)
    float factor = 32.f;

    memset(stream, 0, length);

    int totl;// = spec.freq / (stuff[I()]/2);

    Uint8* buffer = (Uint8*)malloc(length);
    float scale = 1.0f;
    if CONDITION(0) {
        totl = spec.freq / (CHANNELS()[0][I()].freq / 2);
        for(size_t j = 0; j < length; ++j) {
            // channel 0, square
            if(K(0) < (float)CHANNELS()[0][I()].fill/256.f * totl) {
                buffer[j] = factor;
            } else {
                buffer[j] = 0;
            }
            K(0) = (K(0) + 1) % totl;
        }

        mixin(stream, buffer, length, scale);
    }
    if CONDITION(1) {
        totl = spec.freq / (CHANNELS()[1][I()].freq / 2);
        for(size_t j = 0; j < length; ++j) {
            // channel 1, square
            if(K(1) < (float)CHANNELS()[1][I()].fill/256.f * totl) {
                buffer[j] = factor;
            } else {
                buffer[j] = 0;
            }
            K(1) = (K(1) + 1) % totl;
        }

        mixin(stream, buffer, length, scale);
    }
    if CONDITION(2) {
        totl = spec.freq / (CHANNELS()[2][I()].freq / 2);
        for(size_t j = 0; j < length; ++j) {
            // channel 1, square
            if(K(2) < (float)CHANNELS()[2][I()].fill/256.f * totl) {
                buffer[j] = factor;
            } else {
                buffer[j] = 0;
            }
            K(2) = (K(2) + 1) % totl;
        }

        mixin(stream, buffer, length, scale);
    }
    if CONDITION(3) {
        totl = spec.freq / (CHANNELS()[3][I()].freq / 2);
        for(size_t j = 0; j < length; ++j) {
            // channel 1, square
            if(K(3) < (float)CHANNELS()[3][I()].fill/256.f*totl) {
                buffer[j] = factor;
            } else {
                buffer[j] = 0;
            }
            K(3) = (K(3) + 1) % totl;
        }

        mixin(stream, buffer, length, scale);
    }
    if CONDITION(4) {
        totl = spec.freq / (CHANNELS()[4][I()].freq / 2);
        for(size_t j = 0; j < length; ++j) {
            // channel 1, square
            if(K(4) < (float)CHANNELS()[4][I()].fill/256.f*totl) {
                buffer[j] = factor;
            } else {
                buffer[j] = 0;
            }
            K(4) = (K(4) + 1) % totl;
        }

        mixin(stream, buffer, length, scale);
    }
}

void play_music()
{
    SDL_AudioSpec as, spec;
    context ctx;

    SDL_Init(SDL_INIT_AUDIO);
    signal(SIGINT, &siginth);

    // set up AudioSpec
    as.freq = 44100;
    as.format = AUDIO_S16SYS;
    as.channels = 1;
    as.samples = 512;
    as.callback = &audio_callback;
    // pass a pointer with the obtained audio spec back to the callback
    ctx.spec = &spec;
    ctx.i = 0;
    memset(ctx.k, 0, 5 * sizeof(size_t));
    as.userdata = &ctx;

    // start playing music
    SDL_OpenAudio(&as, &spec);
    SDL_PauseAudio(0);

    while(1) {
        SDL_Delay(17); // arbitrary delay => note length
        // prevent the callback from firing mid-modification
        SDL_LockAudio();
        // shift to the next note
        ctx.i = (ctx.i + 1) % maxChannelLen;
        // resume callback
        SDL_UnlockAudio();
    }
}

int main(int argc, char* argv[])
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
                //pwm el(frequency, fill);
                pwm el = { frequency, fill };
                channels[channel].push_back(el);
            }   

            maxChannelLen = std::max(maxChannelLen, channels[channel].size());
        } while(1);
    }

    for(size_t i = 0; i < 5; ++i) {
        printf("channel %ld:\n", i);
        for(size_t j = 0; j < channels[i].size(); ++j) {
            printf("%5d", channels[i][j].freq);
            if(j % 16 == 15 
                    &&  j != channels[i].size() - 1)
            {
                printf("\n");
            }
        }
        printf("\n");
    }

    play_music();

    return 0;
}


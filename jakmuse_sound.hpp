#ifndef JAKMUSE_SOUND_HPP
#define JAKMUSE_SOUND_HPP

// we don't want our function overflowing
#define LIMIT(f) (signed char)(((f > 127.0f) ? 127.0f : ((f < -127.f) ? -127.f : f)))


#define I() (ctx->i)
#define K(I) (ctx->k[I])
#define CHANNELS() (g_channels)
#define CONDITION(N) (CHANNELS()[N].size() > I() && CHANNELS()[N][I()].freq)

#define FILL(idx) (CHANNELS()[idx][I()].fill)
#define FACTOR(idx) ( (float)FILL(idx) / 255.f )
#define SAWTRICHANNEL(idx) do{\
    if CONDITION(idx) {\
        totl = spec.freq / (CHANNELS()[idx][I()].freq / 2);\
        int t1 = (int)(FACTOR(idx) * (float)totl);\
        int t2 = (int)((1.f - FACTOR(idx)) * (float)totl);\
        for(size_t j = 0; j < length; ++j) {\
            if(K(idx) < t1) {\
                buffer[j] = (Uint8)LIMIT( K(idx) / (float)t1 * 2.f * factor - factor);\
            } else {\
                buffer[j] = (Uint8)LIMIT( (t2 - (K(idx) - t1)) / (float)t2 * 2.f * factor - factor);\
            }\
            K(idx) = (K(idx) + 1) % totl;\
        }\
        mixin(stream, buffer, length, scale);\
    }\
}while(0)

#define FILLCHANNEL(idx) do{\
    if CONDITION(idx) {\
        totl = spec.freq / (CHANNELS()[idx][I()].freq / 2);\
        for(size_t j = 0; j < length; ++j) {\
            if(K(idx) < (float)CHANNELS()[idx][I()].fill/256.f * totl) {\
                buffer[j] = factor;\
            } else {\
                buffer[j] = 0;\
            }\
            K(idx) = (K(idx) + 1) % totl;\
        }\
        mixin(stream, buffer, length, scale);\
    }\
}while(0)

#define TRIACHANNEL(idx) do{\
    if CONDITION(idx) { \
        totl = spec.freq / (CHANNELS()[idx][I()].freq / 2); \
        for(size_t j = 0; j < length; ++j) { \
            buffer[j] = (Uint8)LIMIT( (float)abs((float)(K(idx) % totl) - totl/2.f) / (float)(totl/2) * 2.f * factor - factor); \
            K(idx) = (K(idx) + 1) % totl; \
        } \
        mixin(stream, buffer, length, scale); \
    } \
}while(0)

#define NOISECHANNEL(idx) do{\
    static bool noiseInit = 0; \
    static buffer[40960]; \
    static noiseIdx = 0; \
    if(!noiseInit) { \
        SDL_AudioSpec spec; \
        Uint8* buf; \
        Uint32 len; \
        if(NULL == SDL_LoadWAV("noise.wav", &spec, &buf, &len)) abort(); \
        memcpy(buffer, buf, 40960); \
        SDL_FreeWAV(buf); \
    } \
    if CONDITION(idx) { \
        totl = spec.freq / (CHANNELS()[idx][I()].freq / 2); \
        size_t mid = (size_t)LIMIT(FACTOR(idx) * (float)totl); \
        for(size_t j = 0; j < length; ++j) { \
            if(j < mid) buffer[j] = noise[noiseIdx++]; \
            else buffer[j] = 0; \
            noiseIdx %= 40960; \
        } \
        mixin(stream, buffer, length, scale); \
    } \
}while(0)

#endif

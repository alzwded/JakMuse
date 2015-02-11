#include "jakmuse_common.h"
#include <cstdint>
#include <vector>
#include <cstdio>

static void wav_write_header(FILE* f, unsigned samplPerSec, unsigned numSamples)
{
    struct wav_header_s {
        uint32_t ckId;
        uint32_t cksize;
        uint32_t WAVEID;
        uint32_t fmt_ckId;
        uint32_t fmt_cksize;
        uint16_t wFormatTag;
        uint16_t nChannels;
        uint32_t nSamplesPerSec;
        uint32_t nAvgBytesPerSec;
        uint16_t nBlockAlign;
        uint16_t wBitsPerSample;
        uint32_t data_ckId;
        uint32_t data_cksize;
    } header = {
        'RIFF',
        4 + 24 + (8 + 2 /*B per sample*/ * 1 /*N channels*/ * numSamples + 0),
        'WAVE',
        'fmt ',
        16,
        0x0001,
        2,
        samplPerSec,
        samplPerSec * 2 * 1,
        2 * 1,
        16,
        'data',
        2 * 1 * numSamples,
    };

    fwrite(&header, sizeof(wav_header_s), 1, f);
}

static void wav_write_samples(FILE* f, std::vector<short> const& samples)
{
    fwrite(samples.data(), sizeof(short), samples.size(), f);
}

bool wav_write_file(std::string const& filename, std::vector<short> const& samples, unsigned samples_per_second)
{
    FILE* f = fopen(filename.c_str(), "wb");
    if(!f) {
        fprintf(stderr, "failed to open %s for writing\n", filename.c_str());
        return false;
    }

    wav_write_header(f, samples_per_second, samples.size());
    wav_write_samples(f, samples);

    fclose(f);

    return true;
}

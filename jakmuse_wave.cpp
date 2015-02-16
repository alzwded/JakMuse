/*
Copyright (c) 2014-2015, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "jakmuse_common.h"
#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>

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
        'FFIR', // little endian RIFF
        4 + 24 + (8 + 2 /*B per sample*/ * 1 /*N channels*/ * numSamples + 0),
        'EVAW', // little endian WAVE
        ' tmf', // little endian fmt 
        16,
        0x0001,
        1,
        samplPerSec,
        samplPerSec * 2 * 1,
        2 * 1,
        16,
        'atad', // little endian data
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

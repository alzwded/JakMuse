/*
Copyright (c) 2014-2015, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <math.h>
#include <stdio.h>

float sq[65535];
float tr[65535];
float si[65535];

#define PRINT(what, where) do{\
    static char terms[2] = { '\n', ',' }; \
    for(int k = 0; k < 11050; ++k) \
    { \
        fprintf(where, "%f%c", what[k], terms[k < 11049]); \
    } \
}while(0)

int main()
{
    // fqv = 440
    // sr = 11050
    int Ns = 25;
    int fill = 32;

    for(int k = 0; k < 11050 /*65536*/; ++k)
    {
        int lk = k % Ns;
        sq[k] = (lk < Ns * fill / 256)
            ?  1.f
            : -1.f
            ;
        tr[k] = (lk < Ns * fill / 256)
            ? (float)lk * 256 / (Ns * fill) * 2.f - 1.f
            : (float)(Ns - lk) / (Ns - Ns * fill / 256) * 2.f - 1.f
            ;
        // use cos in order to keep triangle and sine in phase
        // for fill = 128
        si[k] = cosf(1.f / Ns * 2.f * 3.14159f * lk
                + 2.f * 3.14159f * fill / 256.f);
    }

    FILE* f = fopen("signals.csv", "w");
    PRINT(sq, f);
    PRINT(tr, f);
    PRINT(si, f);
    fclose(f);

    return 0;
}

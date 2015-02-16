/*
Copyright (c) 2014-2015, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>

FILE* f;

void _8bit()
{
    unsigned char reg = 0xA1;
    size_t cnt = 0;

    printf("%X: ", 0xB1);
    do {
        fprintf(f, "%d, ", reg);
        if(reg & 0x1) {
            reg = (reg >> 1) ^ 0xB1;
        } else {
            reg = (reg >> 1);
        }
        cnt++;
    } while(reg != 0xA1);
    fprintf(f, "%d\n", reg);

    printf("%zd\n", cnt);

    cnt = 0;
    printf("%X: ", 0xB7);
    do {
        fprintf(f, "%d, ", reg);
        if(reg & 0x1) {
            reg = (reg >> 1) ^ 0xB7;
        } else {
            reg = (reg >> 1);
        }
        cnt++;
    } while(reg != 0xA1);
    fprintf(f, "%d\n", reg);

    printf("%zd\n", cnt);
}

void _16bit()
{
    unsigned short reg = 0xA001;
    size_t cnt = 0;

    printf("%X: ", 0xA801);
    do {
        fprintf(f, "%d, ", reg);
        if(reg & 0x1) {
            reg = (reg >> 1) ^ 0xA801;
        } else {
            reg = (reg >> 1);
        }
        cnt++;
    } while(reg != 0xA001);
    fprintf(f, "%d\n", reg);

    printf("%zd\n", cnt);

    cnt = 0;
    printf("%X: ", 0x8255);
    do {
        fprintf(f, "%d, ", reg);
        if(reg & 0x1) {
            reg = (reg >> 1) ^ 0x8255;
        } else {
            reg = (reg >> 1);
        }
        cnt++;
    } while(reg != 0xA001);
    fprintf(f, "%d\n", reg);

    printf("%zd\n", cnt);
}

void guess()
{
    unsigned short reg = 0xA001;
    size_t cnt = 0;
    unsigned short shf = 0x8000;
    while(shf & 0x8000) {
        cnt = 0;
        do {
            if(reg & 0x1) {
                reg = (reg >> 1) ^ shf;
            } else {
                reg = (reg >> 1);
            }
            cnt++;
        } while(reg != 0xA001);

        if(cnt == 127) {
            printf("%X\n", shf);
            break;
        }

        shf++;
    }
}

int main()
{
    f = fopen("data.256", "w");
    _8bit();
    fclose(f);
    f = fopen("data.64k", "w");
    _16bit();
    fclose(f);
    //guess();

    return 0;
}

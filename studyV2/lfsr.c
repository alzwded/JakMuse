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

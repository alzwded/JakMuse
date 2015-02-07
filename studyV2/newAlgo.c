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

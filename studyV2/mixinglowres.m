NSIZE = 172;
A =  zeros(NSIZE);
B =  zeros(NSIZE);
A1 = zeros(NSIZE);
B1 = zeros(NSIZE);
C =  zeros(NSIZE);
D =  zeros(NSIZE);

for i = 1:NSIZE
    j = i * 10;
    A1(i) = sin(440 * j / 11050 * 3.14159);
    B(i) = sin(280 * j / 11050 * 3.14159);
    B1(i) = sin(560 * j / 11050 * 3.14159);
    if(A1(i) > 0)
        A(i) = 1;
    else
        A(i) = 0;
    endif
    if(B(i) > 0)
        B(i) = 1;
    else
        B(i) = 0;
    endif
    if(B1(i) > 0)
        B1(i) = 1;
    else
        B1(i) = 0;
    endif
endfor

S1 = cast(6*NSIZE/9, "uint32");
S2 = cast(7*NSIZE/9, "uint32");
S3 = cast(8*NSIZE/9, "uint32");

A1(S1:NSIZE) = 0;
B (S2:NSIZE) = 0;
A (S3:NSIZE) = 0;

C = A + B + A1 + B1;
D = tanh(C * 0.45);

NPLOTS = 6;
figure(1, 'Position', [20, 20, 1000, 550])
subplot(NPLOTS, 1, 1)
plot(1:NSIZE, A)
title('A440')
subplot(NPLOTS, 1, 2)
plot(1:NSIZE, B)
title('b 280')
subplot(NPLOTS, 1, 3)
plot(1:NSIZE, A1)
title('a1 sin(440)')
subplot(NPLOTS, 1, 4)
plot(1:NSIZE, B1)
title('b1 2 * fqv(b)')
subplot(NPLOTS, 1, 5)
plot(1:NSIZE, C)
title('A + B + C + D')
subplot(NPLOTS, 1, 6)
plot(1:NSIZE, D)
title('tanh(A + B + B + C + D) (signal dampened to 45% beforehand)')

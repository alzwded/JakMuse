X = linspace(0, 4 * pi, 11050);
A = sign(sin(X * 440));

RANGE = 1:50;

B = my_rcfilter(A, 800);
C = my_rcfilter(A, 10000);
D = my_rcfilter(A, 16000);
A = my_rcfilter(A, 0);

figure(1)
set(1, 'Units', 'inches')
set(1, 'Position', [0, 0, 12, 12])
set(1, 'PaperSize', [12, 12])
set(1, 'PaperPositionMode', 'manual')
set(1, 'PaperUnits', 'inches')
set(1, 'PaperPosition', [0, 0, 12, 12])

subplot(2, 2, 1)
plot(A(RANGE))
title('original square signal (440Hz)')
ylim([-1.2, 1.2])

subplot(2, 2, 2)
plot(B(RANGE))
title('filtered signal (800Hz)')
ylim([-1.2, 1.2])

subplot(2, 2, 3)
plot(C(RANGE))
title('filtered signal (10kHz)')
ylim([-1.2, 1.2])

subplot(2, 2, 4)
plot(D(RANGE))
title('filtered signal (16kHz)')
ylim([-1.2, 1.2])

print(1, 'filtered.png', '-dpng', '-color', '-r100', '-S1000,1000')

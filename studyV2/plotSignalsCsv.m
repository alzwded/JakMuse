A = csvread('signals.csv');
RANGE = 1:256;

figure(1)
set(1, 'Units', 'inches')
set(1, 'Position', [0, 0, 10, 5])
set(1, 'PaperSize', [10, 5])
set(1, 'PaperPositionMode', 'manual')
set(1, 'PaperUnits', 'inches')
set(1, 'PaperPosition', [0, 0, 10, 5])

subplot(3, 1, 1)
plot(A(1,RANGE))
title('square (440Hz, 32fill, 11kHz sr, 1s)')
ylim([-1.2, 1.2]);

subplot(3, 1, 2)
plot(A(2,RANGE))
title('triangle')
ylim("auto")

subplot(3, 1, 3)
plot(A(3,RANGE))
title('sine')

print(1, 'signals.png', '-dpng', '-r100', '-color', '-S1000,500')

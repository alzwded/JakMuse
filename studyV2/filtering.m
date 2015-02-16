% Copyright (c) 2014-2015, Vlad Mesco
% All rights reserved.
% 
% Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
% 
% 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
% 
% 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
% 
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

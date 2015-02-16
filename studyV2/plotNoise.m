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
figure(1)
set(1, 'Units', 'inches')
set(1, 'Position', [0, 0, 18, 18])
set(1, 'PaperSize', [18, 18])
set(1, 'PaperPositionMode', 'manual')
set(1, 'PaperUnits', 'inches')
set(1, 'PaperPosition', [0, 0, 18, 18])

A = csvread('data.256');
subplot(2, 1, 1)
plot(1:256, A(1,:))
title('0xB1')
subplot(2, 1, 2)
plot(1:15, A(2,1:15))
title('0xB7')
print(1, 'noise.256.png', '-dpng', '-r100', '-color', '-S1800,1800')

A = csvread('data.64k');
subplot(2, 1, 1)
plot(1:40:65536, A(1,1:40:65536))
title('0xA801')
subplot(2, 1, 2)
plot(1:127, A(2,1:127))
title('0x8255')
print(1, 'noise.64k.png', '-dpng', '-r100', '-color', '-S1800,1800')

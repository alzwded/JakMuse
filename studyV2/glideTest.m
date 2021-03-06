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
F = [100, 280];
%F = [440, 466];
%F = [100, 180];
%F = [415, 440];
%F = [1070, 1140];
%F = [2000, 2800];
%F = [170, 1064];
f0 = F(1);
fN = F(2);
sampleRate = 44100;
gl_t = 0.2;
gl_Ns = cast(sampleRate * 0.2, 'int32');
A = zeros(1, gl_Ns + 1000);
T0 = cast(sampleRate / f0, 'int32');
TN = cast(sampleRate / fN, 'int32');
NsPerOnePeriod = gl_Ns / abs(T0 - TN + 1);
current_numSamples = 0;
counter = NsPerOnePeriod;
Ts = T0:-1:TN; % would be with a 1 step if they were the other way around
idx = 1;
k = 0;
nextStartAt = 1;
lines = [];
for current_numSamples = 1:gl_Ns
  % time step of the sine function
  lk = mod(k, Ts(idx));
  % the allotted time slot
  if(counter < NsPerOnePeriod)
    counter = counter + 1;
  % out of time; sync to the next period
  elseif(lk == 0)
    % reset the counter
    counter = nextStartAt;
    nextStartAt = 1;
    % put the signal in phase
    k = 0; % !!!
    % compute which is the next worthy wavelength
    % 1.03 is a magic number that (somehow?) accounts for the delays
    idx = cast(round(
            (1.03 * current_numSamples + NsPerOnePeriod) / NsPerOnePeriod)
        , 'int32');
    % clamp the value
    idx = min(idx, length(Ts));

    lines = horzcat(lines, [current_numSamples]);
  else
    nextStartAt = nextStartAt + 1;
  endif;
  % generate some signal
  A(current_numSamples) = sin(1/Ts(idx) * 2 * pi * lk);
  % increment time step
  k = k + 1;
  % continue loop
  current_numSamples = current_numSamples + 1;
endfor;

idx
length(Ts)

% continue for a while longer
for current_numSamples = (gl_Ns + 1):(gl_Ns + 1000)
  lk = mod(k, Ts(idx));
  A(current_numSamples) = sin(1/Ts(idx) * 2 * pi * lk);
  k = k + 1;
  current_numSamples = current_numSamples + 1;
endfor

figure(1)
set(1, 'Units', 'inches')
set(1, 'Position', [0, 0, 12, 3])
set(1, 'PaperSize', [12, 3])
set(1, 'PaperPositionMode', 'manual')
set(1, 'PaperUnits', 'inches')
set(1, 'PaperPosition', [0, 0, 12, 3])

%plot(A(5200:5700))
plot(A)
%title('chirping (hopefully) from 440 to 466')
title('chirping from 100 to 280')

line([gl_Ns, gl_Ns], [-1, 1], 'Color', 'r', 'LineWidth', 1)

for i = 1:length(lines)
  line([lines(i), lines(i)], [-1, 1], 'Color', 'g', 'LineWidth', 1)
endfor;

print(1, 'glide.png', '-dpng', '-color', '-r100', '-S1200,300')

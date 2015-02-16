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

function phaseDistortion
  freq = 440;
  plotLen = 200 * 5;

  distortionFuncParams = [ 0.5, 0.3, 0.2, 0.9 ];
  distortionFuncs = [];
  for i = 1:length(distortionFuncParams)
    distortionFuncs = [distortionFuncs; phaseDistortionFunction(freq, distortionFuncParams(i))];
  endfor;

  sigs = [];
  for i = 1:length(distortionFuncs(:,1))
    signl = genSignal(freq, distortionFuncs(i,:), plotLen);
    sigs = [sigs;signl];
  endfor;

  figure(1)
  clf(1)
  set(1, 'Units', 'inches')
  set(1, 'Position', [0, 0, 12, 10])
  set(1, 'PaperSize', [12, 10])
  set(1, 'PaperPositionMode', 'manual')
  set(1, 'PaperUnits', 'inches')
  set(1, 'PaperPosition', [0, 0, 12, 10])

  for i = 1:length(distortionFuncs(:,1))
    subplot(length(distortionFuncs(:,1)), 1, i)
    plot(1:length(sigs(i, 1:plotLen)), sigs(i, 1:plotLen), 'color', 'b')
    hold on;
    plot(1:length(distortionFuncs(i,:)), distortionFuncs(i,:), 'color', 'r')
  endfor

  %print(1, 'phaseDistortion.png', '-dpng', '-color', '-r100', '-S1200x1000')
endfunction

function sig = genSignal(freq, phaseDistortion, plotLen)
  Ns = cast(44100 / freq, 'uint32');
  sig = zeros(1, plotLen);

  for i = 1:plotLen
    lNs = mod(i, Ns);
    sine = sin(phaseDistortion(lNs + 1) * pi + pi);
    %line = 2 * cast((Ns - lNs), 'double') / cast(Ns, 'double') - 1;
    %sig(i) = line * sine;
    sig(i) = sine;
  endfor;
endfunction

function ret = phaseDistortionFunction(baseFreq, zeroPos)
  ret = [];
  base_Ns = (44100 / baseFreq);
  zero_Ns = base_Ns * zeroPos;
  for i = 1:zero_Ns
    ret = [ret, i / zero_Ns - 1];
  endfor;
  for i = 1:(base_Ns - zero_Ns)
    ret = [ret, i / (base_Ns - zero_Ns)];
  endfor;
endfunction

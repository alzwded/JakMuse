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

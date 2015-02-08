function R = my_rcfilter(A, fqv)

  if(fqv == 0)
    R = A;
    return
  endif

  RC = 1 / (2 * pi * fqv);
  timestep = 1 / 11050;
  alpha = timestep / (timestep + RC);
  
  R = zeros(1, length(A));
  
  for i = 2:length(A)
    R(i) = alpha * A(i) + (1 - alpha) * R(i - 1);
  endfor;

endfunction

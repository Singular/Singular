LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  module M = [y3+x2z, xy], [-xy, y2+x2z];
  print(M);
  module K = [x2, xy], [y3, xy], [xy, x];
  print(K);
  matrix A = coeffs(M, K, xy);   // leaving z as variable of interest
  print(A);   // attention: only the first row of M is reproduced by K*A
tst_status(1);$

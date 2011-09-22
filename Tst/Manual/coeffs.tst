LIB "tst.lib"; tst_init();
  ring r;
  poly f = (x+y)^3;
  poly g = xyz+z10y4;
  ideal i = f, g;
  matrix M = coeffs(i, y);
  print(M);
  vector v = [f, g];
  M = coeffs(v, y);
  print(M);
tst_status(1);$

LIB "tst.lib"; tst_init();
  ring r = 0, (x,y), dp;
  poly f0 = y240; poly g0 = y102+1;
  poly h = y342+14x260+7x140y110+2x120y130+y240;
  int d = 260;
  list L = factmodd(h, d, f0, g0); L;
  // check result: next output should be zero
  reduce(h - L[1] * L[2], std(x^(d+1)));
tst_status(1);$

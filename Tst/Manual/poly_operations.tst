LIB "tst.lib"; tst_init();
  ring R=0,(x,y),dp;
  poly f = x3y2 + 2x2y2 + xy - x + y + 1;
  f;
  f + x5 + 2;
  f * x2;
  (x+y)/x;
  f/3x2;
  x5 > f;
  x<=y;
  x>y;
  ring r=0,(x,y),ds;
  poly f = fetch(R,f);
  f;
  x5 > f;
  f[2..4];
  size(f);
  f[size(f)+1]; f[-1];    // monomials out of range are 0
  intvec v = 6,1,3;
  f[v];          // the polynom built from the 1st, 3rd and 6th monomial of f
tst_status(1);$

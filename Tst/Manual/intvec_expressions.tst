LIB "tst.lib"; tst_init();
  intvec v=-1,2;
  intvec w=v,v;         // concatenation
  w;
  w=2:3;                // repetition
  w;
  int k = 3;
  v = 7:k;
  v;
  v=-1,2;
  w=-2..2,v,1;
  w;
  intmat m[3][2] = 0,1,2,-2,3,1;
  m*v;
  typeof(_);
  v = intvec(m);
  v;
  ring r;
  poly f = x2z + 2xy-z;
  f;
  v = leadexp(f);
  v;
tst_status(1);$

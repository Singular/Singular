LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),(c,ds);
  poly f=x2+y+z3;
  vector v=[2*x^10,f];
  leadexp(f);
  leadexp(v);
  leadexp(0);
tst_status(1);$

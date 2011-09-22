LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),(c,ds);
  poly f=2x2+3y+4z3;
  vector v=[2x10,f];
  ideal i=f,z;
  module m=v,[0,0,2+x];
  lead(f);
  lead(v);
  lead(i);
  lead(m);
  lead(0);
tst_status(1);$

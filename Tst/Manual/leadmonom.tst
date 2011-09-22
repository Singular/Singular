LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),(c,ds);
  poly f=2x2+3y+4z3;
  vector v=[0,2x10,f];
  leadmonom(f);
  leadmonom(v);
  leadmonom(0);
tst_status(1);$

LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  poly f=3*(x+2)^3+y;
  poly g=x+y+z;
  resultant(f,g,x);
tst_status(1);$

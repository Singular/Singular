LIB "tst.lib"; tst_init();
  ring r=3,(x,y,z),dp;
  poly f=(x-y)^3*(x+z)*(y-z);
  sqrfree(f);
tst_status(1);$

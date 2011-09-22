LIB "tst.lib"; tst_init();
  ring r=0,(x,y),dp;
  2xy^2 == (2*x*y)^2;
  2xy^2 == 2x*y^2;
  2x*y^2 == 2*x * (y^2);
tst_status(1);$

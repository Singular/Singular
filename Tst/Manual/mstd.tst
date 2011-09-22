LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z,t),dp;
  poly f=x3+y4+z6+xyz;
  ideal j=jacob(f),f;
  j=homog(j,t);j;
  mstd(j);
tst_status(1);$

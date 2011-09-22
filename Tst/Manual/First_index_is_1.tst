LIB "tst.lib"; tst_init();
  ring r;
  ideal i=1,x,z;
  i[2];
  intvec v=1,2,3;
  v[1];
  poly p=x+y+z;
  p[2];
  vector h=[x+y,x,z];
  h[1];
  h[1][1];
tst_status(1);$

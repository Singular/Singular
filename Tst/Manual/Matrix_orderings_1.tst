LIB "tst.lib"; tst_init();
  intmat m[3][3]=1, 0, 0, 0, 1, 0, 0, 0, 1;
  m;
  ring r = 0, (x,y,z), M(m);
  r;
tst_status(1);$

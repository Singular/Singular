LIB "tst.lib"; tst_init();
  ring r;
  matrix m[5][6];
  ncols(m);
  ideal i=x,0,y;
  ncols(i);
  size(i);
tst_status(1);$

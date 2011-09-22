LIB "tst.lib"; tst_init();
  int i=3;
  ring r=0,x,dp;
  poly p;
  listvar();
  kill i,r;
  // the variable `i` does not exist any more
  i;
  listvar();
tst_status(1);$

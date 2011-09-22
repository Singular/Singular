LIB "tst.lib"; tst_init();
  ring r=23,x,dp;
  int save=short;
  short=1;
  2x2,x2;
  short=0;
  2x2,x2;
  short=save;  //resets short to the previous value
tst_status(1);$

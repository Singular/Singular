LIB "tst.lib"; tst_init();
  ring r=0,x,dp;
  number n = 1/2 +1/3;
  n;
  n/2;
  1/2/3;
  1/2 * 1/3;
  n = 2;
  n^-2;
  // the following oddities appear here
  2/(2+3);
  number(2)/(2+3);
  2^-2; // for int's exponent must be non-negative
  number(2)^-2;
  3/4>=2/5;
  2/6==1/3;
tst_status(1);$

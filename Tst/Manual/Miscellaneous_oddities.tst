LIB "tst.lib"; tst_init();
  ring r=32002,x,dp;
  3/2;    // ring division
  3 / 2;  // integer division
  3 div 2;
  number(3) / number(2);
  number a=3;
  number b=2;
  a/b;
  int c=3;
  int d=2;
  c / d;
tst_status(1);$

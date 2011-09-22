LIB "tst.lib"; tst_init();
  ring r=0,x,(c,dp);
  number(3x);
  number(poly(3));
  ideal i=1,2,3,4,5,6;
  print(matrix(i));
  print(matrix(i,3,2));
  vector v=[1,2];
  print(matrix(v));
  module(matrix(i,3,2));
  // generators are columns of a matrix
tst_status(1);$

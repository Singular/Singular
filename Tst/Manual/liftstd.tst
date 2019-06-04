LIB "tst.lib"; tst_init();
  ring R=0,(x,y,z),dp;
  poly f=x3+y7+z2+xyz;
  ideal i=jacob(f);
  matrix T;
  ideal sm=liftstd(i,T);
  sm;
  print(T);
  matrix(sm)-matrix(i)*T;
  module s;
  sm=liftstd(i,T,s);
  print(s);
  sm=liftstd(i,T,s,"std");
  print(s);
  sm=liftstd(i,T,s,"slimgb");
  print(s);
tst_status(1);$

LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  module m=[x,y],[0,0,z];
  print(m*(x+y));
  // this is not distributive:
  print(m*x+m*y);
tst_status(1);$

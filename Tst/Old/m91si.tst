  ring r;
  ideal h1 = x,y,z;
  ideal h2 = x;
  module m=modulo(h1,h2);
  print(m);
LIB "tst.lib";tst_status(1);$

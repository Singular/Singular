LIB "tst.lib"; tst_init();
  ring r;
  ideal h1=x,y,z;
  ideal h2=x;
  module m=modulo(h1,h2);
  print(m);
tst_status(1);$

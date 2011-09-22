LIB "tst.lib"; tst_init();
  ring r=0,(x,y),dp;
  map f=r,y,x;    // the map f permutes the variables
  f;
  poly p=x+2y3;
  f(p);
  map g=f(f);    // the map g defined as  f^2 is the identity
  g;
  g(p) == p;
tst_status(1);$

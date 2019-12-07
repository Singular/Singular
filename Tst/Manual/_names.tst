LIB "tst.lib"; tst_init();
  int i=9;
  ring r;
  poly f;
  package p;
  int j; exportto(p,j);
  poly g;
  setring r;
  list l=names();
  l[1..3];
  names(r);
  names(p);
tst_status(1);$

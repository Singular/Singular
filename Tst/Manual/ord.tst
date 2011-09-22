LIB "tst.lib"; tst_init();
  ring r=7,(x,y),wp(2,3);
  ord(0);
  poly f=x2+y3;  // weight on y is 3
  ord(f),deg(f);
  ring R=7,(x,y),ws(2,3);
  poly f=x2+y3;
  ord(f),deg(f);
  vector v=[x2,y];
  ord(v),deg(v);
tst_status(1);$

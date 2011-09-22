LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),lp;
  deg(0);
  deg(x3+y4+xyz3);
  ring rr=7,(x,y),wp(2,3);
  poly f=x2+y3;
  deg(f);
  ring R=7,(x,y),ws(2,3);
  poly f=x2+y3;
  deg(f);
  vector v=[x2,y];
  deg(v);
tst_status(1);$

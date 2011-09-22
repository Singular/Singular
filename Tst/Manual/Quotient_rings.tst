LIB "tst.lib"; tst_init();
  ring r=32003,(x,y),dp;
  poly f=x3+yx2+3y+4;
  qring q=std(maxideal(2));
  basering;
  poly g=fetch(r, f);
  g;
  reduce(g,std(0));
tst_status(1);$

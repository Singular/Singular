LIB "tst.lib"; tst_init();
  ring h1=32003,(t,x,y,z),dp;
  ideal i=
  9x8+y7t3z4+5x4y2t2+2xy2z3t2,
  9y8+7xy6t+2x5y4t2+2x2yz3t2,
  9z8+3x2y3z2t4;
  intvec e=weight(i);
  e;
  ring r=32003,(a,b,c,d),wp(e);
  map f=h1,a,b,c,d;
  ideal i0=std(f(i));
tst_status(1);$

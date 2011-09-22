LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  monomial(intvec(2,3));
  monomial(intvec(2,3,0,1));
  leadexp(monomial(intvec(2,3,0,1)));
tst_status(1);$

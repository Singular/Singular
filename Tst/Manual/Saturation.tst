LIB "tst.lib"; tst_init();
  LIB "elim.lib";         // loading library elim.lib
  ring r2 = 32003,(x,y,z),dp;
  poly f = x^11+y^5+z^(3*3)+x^(3+2)*y^(3-1)+x^(3-1)*y^(3-1)*z3+
    x^(3-2)*y^3*(y^2)^2;
  ideal j=jacob(f);
  sat_with_exp(j+f,maxideal(1));
  // list the variables defined so far:
  listvar();
tst_status(1);$

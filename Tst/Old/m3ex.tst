  LIB "elim.lib";         // loading library elim.lib
  // you should get the information that elim.lib has been loaded
  // together with some other libraries which are needed by it
  option(noprot);         // no protocol
  ring r2 = 32003,(x,y,z),dp;
  poly f = x^11+y^5+z^(3*3)+x^(3+2)*y^(3-1)+x^(3-1)*y^(3-1)*z3+
    x^(3-2)*y^3*(y^2)^2;
  ideal j=jacob(f);
  sat_with_exp(j+f,maxideal(1));
  // list and kill the variables defined so far:
  listvar();
  kill r2;
LIB "tst.lib";tst_status(1);$

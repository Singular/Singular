  ring r= 32003,(x,y),ds;
  poly f = (x^3+y^5)^2+x^2*y^7;
  ideal i = std(jacob(f));
  mult(i);
  mult(std(f));
LIB "tst.lib";tst_status(1);$

  ring rh3=32003,(w,x,y,z),(dp,C);
  poly f= x11+y10+z9+x5*y2+x2*y2*z3+x*y^3*(y2+x)^2;
  ideal j= homog(jacob(f),w);
  list jj=nres(j,0);
  regularity(jj);
LIB "tst.lib";tst_status(1);$

LIB "tst.lib"; tst_init();
  ring rh3=32003,(w,x,y,z),(dp,C);
  poly f=x11+y10+z9+x5y2+x2y2z3+xy3*(y2+x)^2;
  ideal j=homog(jacob(f),w);
  def jr=res(j,0);
  regularity(jr);
  // example for upper bound behaviour:
  list jj=jr;
  regularity(jj);
  jj=nres(j,0);
  regularity(jj);
  jj=minres(jj);
  regularity(jj);
tst_status(1);$

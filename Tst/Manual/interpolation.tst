LIB "tst.lib"; tst_init();
  ring r=0,(x,y),dp;
  ideal p_1=x,y;
  ideal p_2=x+1,y+1;
  ideal p_3=x+2,y-1;
  ideal p_4=x-1,y+2;
  ideal p_5=x-1,y-3;
  ideal p_6=x,y+3;
  ideal p_7=x+2,y;
  list l=p_1,p_2,p_3,p_4,p_5,p_6,p_7;
  intvec v=2,1,1,1,1,1,1;
  ideal j=interpolation(l,v);
  // generator of degree 3 gives the equation of the unique
  // singular cubic passing
  // through p_1,...,p_7 with singularity at p_1
  j;
  // computes values of generators of j at p_4, results should be 0
  subst(j,x,1,y,-2);
  // computes values of derivatives d/dx of generators at (0,0)
  subst(diff(j,x),x,0,y,0);
tst_status(1);$

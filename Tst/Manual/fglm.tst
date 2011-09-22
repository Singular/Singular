LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal i=y3+x2, x2y+x2, x3-x2, z4-x2-y;
  option(redSB);   // force the computation of a reduced SB
  i=std(i);
  vdim(i);
  ring s=0,(z,x,y),lp;
  ideal j=fglm(r,i);
  j;
tst_status(1);$

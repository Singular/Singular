  ring r=0,(x,y),ds;
  ideal i=x2+y2,x2-y2;
  ideal j=std(i);
  vdim(j);
LIB "tst.lib";tst_status(1);$

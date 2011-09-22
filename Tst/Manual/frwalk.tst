LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal i=y3+x2, x2y+x2, x3-x2, z4-x2-y;
  i=std(i);
  ring s=0,(x,y,z),lp;
  ideal j=frwalk(r,i);
  j;
tst_status(1);$

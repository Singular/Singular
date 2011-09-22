LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),lp;
  ideal i=y3+x2,x2y+x2,x3-x2,z4-x2-y;
  option(redSB);   // force the computation of a reduced SB
  i=std(i);
  poly p=reduce(x+yz2+z10,i);
  ideal j=fglmquot(i,p);
  j;
tst_status(1);$

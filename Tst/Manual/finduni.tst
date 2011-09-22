LIB "tst.lib"; tst_init();
  ring  r=0,(x,y,z), dp;
  ideal i=y3+x2,x2y+x2,z4-x2-y;
  option(redSB);  // force computation of reduced basis
  i=std(i);
  ideal k=finduni(i);
  print(k);
tst_status(1);$

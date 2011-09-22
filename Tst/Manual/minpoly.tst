LIB "tst.lib"; tst_init();
  //(Q[i]/(i^2+1))[x,y,z]:
  ring Cxyz=(0,i),(x,y,z),dp;
  minpoly=i^2+1;
  i2;  //this is a number, not a poly
tst_status(1);$

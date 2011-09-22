LIB "tst.lib"; tst_init();
  ring R=0,x,dp;
  matrix m[2][3]=1,2,3,4,5,6;
  print(m);
  print(transpose(m));
tst_status(1);$

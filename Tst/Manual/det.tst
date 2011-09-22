LIB "tst.lib"; tst_init();
  ring r=7,(x,y),wp(2,3);
  matrix m[3][3]=1,2,3,4,5,6,7,8,x;
  det(m);
tst_status(1);$

LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal I=y,z,x,x3,xz;
  sortvec(I);
tst_status(1);$

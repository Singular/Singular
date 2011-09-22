LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),(c,dp);
  ideal I=xyz,x2z;
  facstd(I);
  facstd(I,x);
tst_status(1);$

  ring r= 32003,(x,y,z),(c,dp);
  ideal I=xyz,x2z;
  facstd(I);
  facstd(I,x);
LIB "tst.lib";tst_status(1);$

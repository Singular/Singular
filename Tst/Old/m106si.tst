  ring r1=32003,(x,y,z,w),lp;
  ring r=32003,(x,y,z),dp;
  ideal i=x,y,z;
  ideal i1=x,y;
  map f=r1,i;
  setring r1;
  ideal i1=preimage(r,f,i1);
  i1;
LIB "tst.lib";tst_status(1);$

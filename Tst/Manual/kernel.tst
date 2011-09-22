LIB "tst.lib"; tst_init();
  ring r1=32003,(x,y,z,w),lp;
  ring r=32003,(x,y,z),dp;
  ideal i=x,y,z;
  map f=r1,i;
  setring r1;
  // the kernel of f
  kernel(r,f);
tst_status(1);$

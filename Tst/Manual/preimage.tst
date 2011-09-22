LIB "tst.lib"; tst_init();
  ring r1=32003,(x,y,z,w),lp;
  ring r=32003,(x,y,z),dp;
  ideal i=x,y,z;
  ideal i1=x,y;
  ideal i0=0;
  map f=r1,i;
  nameof (preimage (f));
  setring r1;
  ideal i1=preimage(r,f,i1);
  i1;
  // the kernel of f
  preimage(r,f,i0);
  // or, use:
  kernel(r,f);
tst_status(1);$

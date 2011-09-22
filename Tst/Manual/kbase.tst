LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),ds;
  ideal i=x2,y2,z;
  kbase(std(i));
  i=x2,y3,xyz;  // quotient not finite dimensional
  kbase(std(i),2);
tst_status(1);$

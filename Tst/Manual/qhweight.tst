LIB "tst.lib"; tst_init();
  ring h1=32003,(t,x,y,z),dp;
  ideal i=x4+y3+z2;
  qhweight(i);
tst_status(1);$

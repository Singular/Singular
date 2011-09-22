LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal i=x*y*z-1,x+y+z,x*y+x*z+y*z; // cyclic 3
  janet(i);
tst_status(1);$

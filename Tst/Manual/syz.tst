LIB "tst.lib"; tst_init();
  ring R=0,(x,y),(c,dp);
  ideal i=x,y;
  syz(i);
tst_status(1);$

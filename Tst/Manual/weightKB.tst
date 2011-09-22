LIB "tst.lib"; tst_init();
  ring R=0, (x,y), wp(1,2);
weightKB(ideal(0),3,intvec(1,2));
tst_status(1);$

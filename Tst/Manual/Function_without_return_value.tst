LIB "tst.lib"; tst_init();
  ring r;
  ideal i=x2+y2,x;
  i=std(i);
  degree(i);        // degree has no return value but prints output
tst_status(1);$

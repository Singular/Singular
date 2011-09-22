LIB "tst.lib"; tst_init();
  TRACE=1;
  LIB "general.lib";
  sum(1..100);
tst_status(1);$

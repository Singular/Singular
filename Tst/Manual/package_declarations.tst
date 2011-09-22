LIB "tst.lib"; tst_init();
  package Test;
  int Test::i;
  listvar();
  listvar(Test);
  package dummy = Test;
  kill Test;
  listvar(dummy);
tst_status(1);$

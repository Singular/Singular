LIB "tst.lib"; tst_init();
  intmat(intvec(1));
  intmat(intvec(1), 1, 2);
  intmat(intvec(1,2,3,4), 2, 2);
  intmat(_, 2, 3);
  intmat(_, 2, 1);
tst_status(1);$

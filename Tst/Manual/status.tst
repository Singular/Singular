LIB "tst.lib"; tst_init();
  link l=":w example.txt";
  status(l,"write");
  open(l);
  status(l,"write","ready");
  close(l);
tst_status(1);$

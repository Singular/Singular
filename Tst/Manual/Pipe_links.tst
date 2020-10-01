LIB "tst.lib"; tst_init();
  link l="|: echo bla";
  open(l); l;
  read(l);
  close(l);
  l;
tst_status(1);$

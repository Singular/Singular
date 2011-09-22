LIB "tst.lib"; tst_init();
  link l="|: date";
  open(l); l;
  read(l);
  l;
  close(l);
tst_status(1);$

LIB "tst.lib"; tst_init();
  link l="|: date --date='@0' --rfc-3339='seconds'";
  open(l); l;
  read(l);
  l;
  close(l);
tst_status(1);$

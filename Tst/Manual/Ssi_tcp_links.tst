LIB "tst.lib"; tst_init();
  int i=7;
  link l = "ssi:fork";      // fork link declaration
  open(l); l;

  write(l,quote(i)); // Child inherited vars and their values
  read(l);
  close(l);          // shut down forked child
tst_status(1);$

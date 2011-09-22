LIB "tst.lib"; tst_init();
  link l="DBM:rw example";
  write(l,"1","abc");
  write(l,"3","XYZ");
  write(l,"2","ABC");
  l;
  close(l);
  // read all keys (till empty string):
  read(l);
  read(l);
  read(l);
  read(l);
  // read data corresponding to key "1"
  read(l,"1");
  // read all data:
  read(l,read(l));
  read(l,read(l));
  read(l,read(l));
  // close
  close(l);
tst_status(1);$

LIB "tst.lib"; tst_init();
  link l=":w example.txt";
  int i=22;          // cf. ASCII links for explanation
  string s="An int follows:";
  write(l,s,i);
  l;
  close(l);          //
  read(l);
  close(l);
tst_status(1);$

  link l=":w example.txt";
  status(l,"write");
  open(l);
  status(l,"write");
  close(l);
LIB "tst.lib";tst_status(1);$

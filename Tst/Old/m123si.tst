  link l=":w example.txt";
  status(l,"write");
  open(l);
  status(l,"write");
  close(l);
  system("sh","rm example.txt");
LIB "tst.lib";tst_status(1);$

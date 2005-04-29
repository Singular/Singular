  int i=3;
  dump(":w example.txt");
  kill i;
  getdump("example.txt");
  i;
  system("sh","rm example.txt");
LIB "tst.lib";tst_status(1);$

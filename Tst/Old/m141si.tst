  int i=3;
  dump(":w example.txt");
  kill i;
  getdump("example.txt");
  i;
LIB "tst.lib";tst_status(1);$

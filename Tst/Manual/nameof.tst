LIB "tst.lib"; tst_init();
  int i=9;
  string s=nameof(i);
  s;
  nameof(s);
  nameof(i+1); //returns the empty string:
  nameof(basering);
  basering;
  ring r;
  nameof(basering);
tst_status(1);$

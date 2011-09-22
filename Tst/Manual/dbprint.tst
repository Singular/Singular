LIB "tst.lib"; tst_init();
  int debug=0;
  intvec i=1,2,3;
  dbprint(debug,i);
  debug=1;
  dbprint(debug,i);
  voice;
  printlevel;
  dbprint(i);
tst_status(1);$

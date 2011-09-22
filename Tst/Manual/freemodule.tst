LIB "tst.lib"; tst_init();
  ring r=32003,(x,y),(c,dp);
  freemodule(3);
  matrix m=freemodule(3); // generates the 3x3 unit matrix
  print(m);
tst_status(1);$

LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  print(char_series(ideal(xyz,xz,y)));
tst_status(1);$

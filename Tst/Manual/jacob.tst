LIB "tst.lib"; tst_init();
  ring R;
  poly f = x2yz + xy3z + xyz5;
  ideal i = jacob(f); i;
  matrix m = jacob(i);
  print(m);
  print(jacob(m));
tst_status(1);$

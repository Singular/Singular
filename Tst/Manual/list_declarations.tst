LIB "tst.lib"; tst_init();
  list l=1,"str";
  l[1];
  l[2];
  ring r;
  listvar(r);
  ideal i = x^2, y^2 + z^3;
  l[3] = i;
  l;
  listvar(r);     // the list l belongs now to the ring r
tst_status(1);$

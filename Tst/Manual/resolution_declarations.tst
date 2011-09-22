LIB "tst.lib"; tst_init();
  ring R;
  ideal i=z2,x;
  resolution re=res(i,0);
  re;
  betti(re);
  list l = re;
  l;
tst_status(1);$

LIB "tst.lib"; tst_init();
  ring R;
  int n=3;
  ideal j(3);
  ideal j(n);     // is equivalent to the above
  ideal j(2)=x;
  j(2..3);
  ring r=0,(x(1..2)(1..3)(1..2)),dp;
  r;
tst_status(1);$

  ring r = 0, (x(1..5)), ds;
  matrix m[2][4]= x(1), x(2), x(3), x(4), x(2), x(3), x(4), x(5);
  print(m);
  ideal j = minor(m, 2);
  j;
LIB "tst.lib";tst_status(1);$

ring r = 32003, (x,y), ds;
poly p = y2+y3+x2y2;
ideal i = y2+x10y, x22;
ideal j = std(i);
// noether = x30;
reduce(p, j, 2);
reduce(p, j, 1);
reduce(p,j);
reduce(p - reduce(p, j, 2), j);
reduce(p - reduce(p, j), j);

LIB "tst.lib"; tst_status(1);$

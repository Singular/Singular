LIB "tst.lib"; tst_init();

ring rng = integer,(x,y),dp;
ideal j= x, 5*y, -3*x-4*y+2;
interred(j);

ring rng = integer,(x,y),dp;
ideal j= x, 3*y, x-2*y;
interred(j);

tst_status(1);$


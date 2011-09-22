LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r  = 32003,(x,y),ds;
ideal i = x8,y8;
ideal j = (x+y)^4;
i       = intersect(i,j);
poly f  = xy;
is_reg(f,i);
tst_status(1);$

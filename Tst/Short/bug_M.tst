LIB "tst.lib";
LIB "poly.lib";
ring r = (32003),(a,b,c),(M(1,2,3,4,5,6,7,8,9), C);
// this relveals a bug in 1-2-3
ac2 + b2c;
ideal id = cyclic(3);
int db_print = 1;
tst_AddIdeal(id, 4);

tst_status(1);$

LIB "tst.lib";
tst_init();

// error in InitSSpecial -- did not put elements of Q into T
// fixed in 1-3-7
ring r;
qring p2 = std(x2);
ideal i = y3+x;
std(i, xy2);

tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "zeroset.lib";
ring R = (0,a), x, lp;
minpoly = a2+1;
poly f =  x4 - 2;
poly g = x - a;
list qr = Quotient(f, g);
qr;
qr[1]*g + qr[2] - f;
tst_status(1);$

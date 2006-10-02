LIB "tst.lib";
tst_init();
// error in resultant: wrong number of var x sent to factory
ring R1 = 0, (x, y), lp;
poly f = x+y;
poly g = x^2;
resultant(f, g, x);
ring R2 = (0, a), (x, y), lp;
minpoly = a;
poly f = x+y;
poly g = x^2;
resultant(f, g, x);



tst_status(1);$

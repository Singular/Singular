LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(a,b,c,d),dp;
ring R = freeAlgebra(r, 5);
(a + b) * (c + d); // a*c + a*d + b*c + b*d
(a + b) * (b + a); // a*a + a*b + b*a + b*b
tst_status(1);$

LIB "tst.lib"; tst_init();
/* the following code is (one) reason, why imap.tst fails on 64bit machines. It runs endlessly with regular Singular in spielwiese and crashes under Singularg, while running smoothly on 32bit machines. */
ring s2 = (32003, a,b), (x, y), lp;
poly p = 1 + a + a2 + b + ab + ax + ay + bx + x + y + xy;
ring f1 = (7, a) , (b, x, y), lp; minpoly = a2 + 1;
imap(s2, p);
tst_status(1);$


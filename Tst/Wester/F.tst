LIB "tst.lib"; tst_init();


ring R = 0, (x, y), lp;
poly g;
poly p = (x^2 - 3*x*y + y^2)^4*(3*x - 7*y + 2)^5;
poly q = (x^2 - 3*x*y + y^2)^3*(3*x - 7*y - 2)^6;

tst_InitTimer();

g = gcd(p, q);

tst_ReportTimer("F");

factorize(g);
kill R;

tst_status(1); $

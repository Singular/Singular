LIB "tst.lib"; tst_init();

ring R = 181, (x, y, z), lp;

poly p = (7*y*x^2*z^2 - 3*x*y*z + 11*(x+1)*y2 + 5*z + 1)^4 *
         (3*x - 7*y + 2*z - 3)^5;
poly q = (7*y*x^2*z^2 - 3*x*y*z + 11*(x+1)*y2 + 5*z + 1)^3 *
         (3*x - 7*y + 2*z - 3)^6;

tst_InitTimer();

poly g = gcd(p, q);

tst_ReportTimer("Gp");
  
factorize(g);
kill R;

tst_status(1); $


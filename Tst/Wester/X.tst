LIB "tst.lib"; tst_init();

ring R = (17027, t), (x, y, z), lp;
minpoly = t^2 + 1;

poly g;


poly p4 = (7*t*y*x^2*z^2 - 3*t + t*x*y*z + 11*(x + 1 + t)*y^2 + 5*z + t
+ 1)^4*(3*t*x - 7*t*y + 2*z - 3*t + 1)^5;
poly q4 = (7*t*y*x^2*z^2 - 3*t + t*x*y*z + 11*(x + 1 + t)*y^2 + 5*z + t
+ 1)^3*(3*t*x - 7*t*y + 2*z + 3*t - 1)^6;

tst_InitTimer();

g = gcd(p4, q4);

tst_ReportTimer("X");

g;

tst_InitTimer();
factorize(g);
tst_ReportTimer("X+");

kill R;

tst_status(1); $



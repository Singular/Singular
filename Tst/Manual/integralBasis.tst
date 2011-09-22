LIB "tst.lib"; tst_init();
LIB "integralbasis.lib";
printlevel = printlevel+1;
ring s = 0,(x,y),dp;
poly f = y5-y4x+4y2x2-x4;
list l = integralBasis(f, 2);
l;
// The integral basis of the integral closure of Q[x] in Q(x,y) consists
// of the elements of l[1] divided by the polynomial l[2].
printlevel = printlevel-1;
tst_status(1);$

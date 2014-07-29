LIB "tst.lib";
tst_init();

LIB("primdecint.lib");
ring rng = integer,(t,x),dp;
short = 0;
ideal I = t-3,x-4;
ideal J = 3*t,7;
std( intersectZ(I,J) );


tst_status(1); $


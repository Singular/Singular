LIB "tst.lib";
tst_init();

//input script that runs infinitely (resultatente)
LIB "primdec.lib";
ring r=11,(x,y,z,w,u),dp;
ideal i=(x^3 - 5*y*z + u^2, x - y^2 + 3*z^2, w^2 + 2*u^3*y, y - u^2 +
 z*x);
primdecSY(i);

tst_status(1);$

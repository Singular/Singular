LIB "tst.lib"; tst_init();
LIB "monomialideal.lib";
ring R = 0,(x,y,z,t),dp;
poly f = x^3*z^5*t^2;
poly g = y^6*z^3*t^3;
lcmMon(f,g);
tst_status(1);$

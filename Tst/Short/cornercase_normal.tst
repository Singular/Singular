LIB "tst.lib";
tst_init();

LIB "normal.lib";
ring S = 0,(x,y),dp;
ideal i = y^3 + y - x^4; // is already normal
locNormal(i);

tst_status(1);$

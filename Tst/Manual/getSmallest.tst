LIB "tst.lib"; tst_init();
LIB "normal.lib";
printlevel = printlevel+1;
ring s = 0,(x,y),dp;
ideal J = x3-y, y5, x2-y2+1;
getSmallest(J);
printlevel = printlevel-1;
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "solve.lib";
ring r1 = 0,(x),lp;
// determine f with deg(f) = 4 and
// v = values of f at points 3^0, 3^1, 3^2, 3^3, 3^4
ideal v=16,0,11376,1046880,85949136;
interpolate( 3, v, 4 );
tst_status(1);$

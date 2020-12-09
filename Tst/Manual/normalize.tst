LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(x,y,z),ls;
poly f = 2x5+3y2+4z3;
normalize(f);
module m=[9xy,0,3z3],[4z,6y,2x];
normalize(m);
ring s = 0,(x,y,z),(c,ls);
module m=[9xy,0,3z3],[4z,6y,2x];
normalize(m);
tst_status(1);$

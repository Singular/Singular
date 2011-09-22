LIB "tst.lib"; tst_init();
LIB "algebra.lib";
ring r = 0,(a,b,c),dp;
ring s = 0,(x,y,z),dp;
ideal i= xy;
map phi= r,(xy)^3+x2+z,y2-1,z3;
mapIsFinite(phi,r,i);
tst_status(1);$

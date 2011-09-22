LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),dp;
ideal G = x2y+x2,y3+xyz,xyz2+z4;
pairset(G);"";
module T = [x2y3-z2,x2y],[x2y2+z2,x2-y2],[x2y4+z3,x2+y2];
pairset(T);
tst_status(1);$

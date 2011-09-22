LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring R1=0,(x,y,z),(c,dp);
ideal S = x2y+x2,y3+xyz;
poly h = x2y+xyz;
list P = pairset(S)[1];
P;"";
updatePairs(P,S,h);"";
module T = [x2y3-z2,x2y],[x2y4+z3,x2+y2];
P = pairset(T)[1];
P;"";
updatePairs(P,T,[x2+x2y,y3+xyz]);
tst_status(1);$

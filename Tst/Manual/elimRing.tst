LIB "tst.lib"; tst_init();
LIB "elim.lib";
ring R = 0,(x,y,z,u,v),(c,lp);
def P = elimRing(yu);  P;
intvec w = 1,1,3,4,5;
elimRing(yu,w);
ring S =  (0,a),(x,y,z,u,v),ws(1,2,3,4,5);
minpoly = a2+1;
qring T = std(ideal(x+y2+v3,(x+v)^2));
def Q = elimRing(yv)[1];
setring Q; Q;
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r1 = 0,(x,y,z),(C,Dp);
def r2 = makeModElimRing(r1); setring r2; r2;   kill r2;
ring r3 = 0,(z,t),(wp(2,3),c);
def r2 = makeModElimRing(r3); setring r2; r2; kill r2;
ring r4 = 0,(z,t,u,w),(a(1,2),C,wp(2,3,4,5));
def r2 = makeModElimRing(r4); setring r2; r2;
tst_status(1);$

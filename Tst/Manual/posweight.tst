LIB "tst.lib"; tst_init();
LIB "spcurve.lib";
ring r=32003,(x(1),x(2),x(3)),ds;
ideal curve=(x(3)-x(1)^2)*x(3),(x(3)-x(1)^2)*x(2),x(2)^2-x(1)^7*x(3);
matrix M=isCMcod2(curve);
list l=matrixT1(M,3);
def rneu=posweight(l[1],std(l[2]),0);
setring rneu;
pmat(posw[1]);
posw[2];
tst_status(1);$

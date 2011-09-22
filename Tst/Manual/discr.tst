LIB "tst.lib"; tst_init();
LIB "spcurve.lib";
ring r=32003,(x(1),x(2),x(3)),ds;
ideal curve=x(1)*x(2),x(1)*x(3),x(2)*x(3);
matrix M=isCMcod2(curve);
list l=matrixT1(M,3);
def rneu=semiCMcod2(l[1],std(l[2]));
setring rneu;
discr(semi,3);
tst_status(1);$

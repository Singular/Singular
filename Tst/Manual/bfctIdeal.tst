LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring @D = 0,(x,y,Dx,Dy),dp;
def D = Weyl();
setring D;
ideal I = 3*x^2*Dy+2*y*Dx,2*x*Dx+3*y*Dy+6; I = std(I);
intvec w1 = 0,1;
intvec w2 = 2,3;
bfctIdeal(I,w1);
bfctIdeal(I,w2,0,1);
ideal J = I[size(I)]; // J is not holonomic by construction
bfctIdeal(J,w1); //  b-function of D/J w.r.t. w1 is non-zero
bfctIdeal(J,w2); //  b-function of D/J w.r.t. w2 is zero
tst_status(1);$

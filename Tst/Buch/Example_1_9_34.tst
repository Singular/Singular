LIB "tst.lib";
tst_init();

// ================================ example 1.9.34 ============================

LIB "gkdim.lib";
LIB "ncalg.lib";
def A = makeUsl(2); // set up U(sl_2)
setring A;
ideal I  = e2,f;
ideal LI = std(I);
GKdim(LI);
ideal TI = twostd(I);
GKdim(TI);
ideal Z = 4*e*f + h^2 - 4*h;
Z = std(Z);
GKdim(Z);
ring B = 0,(a),dp;
def  C = A + B;
setring C;
ideal I = e^3, f^3, h^3-4*h, 4*e*f+h^2-2*h - a;
I = std(I);
GKdim(I);
ideal J = eliminate(I,e*f*h);
GKdim(J);
setring A;
resolution F = nres(LI,0); // we computed it before
GKdim(F[1]); // this is LI itself
GKdim(F[2]);
GKdim(F[3]);

tst_status(1);$


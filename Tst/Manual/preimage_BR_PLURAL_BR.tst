LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
ring   R = 0,a,dp;
def Usl2 = makeUsl2();
setring Usl2;
poly  C = 4*e*f+h^2-2*h;
// C is a central element of U(sl2)
ideal I = e^3, f^3, h^3-4*h;
ideal Z = 0;  // zero
ideal J = twostd(I); // two-sided GB
ideal K = std(I);    // left GB
map Phi = R,C;  // phi maps a (in R) to C (in U(sl2))
setring R;
ideal PreJ = preimage(Usl2,Phi,J);
// the central character of J
PreJ;
factorize(PreJ[1],1);
// hence, there are two simple characters for J
ideal PreK = preimage(Usl2,Phi,K);
// the central character of K
PreK;
factorize(PreK[1],1);
// hence, there are three simple characters for K
preimage(Usl2, Phi, Z);  // kernel pf phi

tst_status(1);$

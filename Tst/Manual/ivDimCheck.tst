LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
R;
setring R; // sets basering to Letterplace ring
//some intmats, which contain monomials in intvec representation as rows
intmat I1 [2][2] = 1,1,2,2; intmat I2 [1][3]  = 1,2,1;
intmat J1 [1][2] =  1,1; intmat J2 [2][3] = 2,1,2,1,2,1;
print(I1);
print(I2);
print(J1);
print(J2);
list G = I1,I2;// ideal, which is already a Groebner basis
list I = J1,J2; // ideal, which is already a Groebner basis and which
ivDimCheck(G,2); // invokes the procedure, factor is of finite K-dimension
ivDimCheck(I,2); // invokes the procedure, factor is not of finite K-dimension
tst_status(1);$

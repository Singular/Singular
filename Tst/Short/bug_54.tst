LIB "tst.lib";
tst_init();

// bug in coefss-3
ring r=0,(x,y,z),dp;
poly f = x4+2x2+x3y4+y5+z7;
matrix T;
matrix M = coeffs(f,y,T);
T;
// should be true:
T*M == f;
 //---------------
ring R=0,(x,y,z),dp;
ideal I = x3y4+z7+y5+x4+2x2,x2y+y3;
matrix TT;
matrix MM = coeffs(I,y);
print (MM);
matrix NN = coeffs(I,y,TT);
// should be similar:
TT*NN;
I;

tst_status(1);$

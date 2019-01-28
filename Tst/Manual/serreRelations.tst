LIB "tst.lib"; tst_init();
LIB "freegb.lib";
intmat A[3][3] =
2, -1, 0,
-1, 2, -3,
0, -1, 2; // G^1_2 Cartan matrix
ring r = 0,(f1,f2,f3),Dp;
int uptodeg = 5;
def R = freeAlgebra(r,uptodeg);
setring R;
ideal I = serreRelations(A,1); I = simplify(I,1+2+8);
I;
tst_status(1);$

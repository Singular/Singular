LIB "tst.lib"; tst_init();
LIB "freegb.lib";
intmat A[2][2] =
2, -1,
-1, 2; // A_2 = sl_3 Cartan matrix
ring r = 0,(f1,f2,h1,h2,e1,e2),Dp;
ideal negroots = f1,f2; ideal cartans = h1,h2; ideal posroots = e1,e2;
int uptodeg = 5;
def RS = fullSerreRelations(A,negroots,cartans,posroots,uptodeg);
setring RS; fsRel;
tst_status(1);$

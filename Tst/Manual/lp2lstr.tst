LIB "tst.lib"; tst_init();
LIB "freegb.lib";
intmat A[2][2] = 2, -1, -1, 2; // sl_3 == A_2
ring r = 0,(f1,f2),dp;
def R = makeLetterplaceRing(3);
setring R;
ideal I = serreRelations(A,1);
lp2lstr(I,r);
setring r;
lst2str(@LN,1);
tst_status(1);$

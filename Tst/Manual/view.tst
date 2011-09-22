LIB "tst.lib"; tst_init();
LIB "control.lib";
ring r;
list L;
matrix M[1][3] = x2+x,y3-y,z5-4z+7;
L[1] = "a matrix:";
L[2] = M;
L[3] = "an ideal:";
L[4] = ideal(M);
view(L);
tst_status(1);$

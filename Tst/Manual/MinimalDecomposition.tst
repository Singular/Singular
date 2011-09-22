LIB "tst.lib"; tst_init();
LIB "rinvar.lib";
ring R = 0, (s(1..2), t(1..2)), dp;
poly h = s(1)*(t(1) + t(1)^2) +  (t(2) + t(2)^2)*(s(1)^2 + s(2));
matrix M = MinimalDecomposition(h, 2, 2);
M;
M[1,1]*M[2,1] + M[1,2]*M[2,2] - h;
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring R = 0, (x,y), dp;
intmat M[2][2]=
1, 1,
0, 2;
intmat T[2][5]=
1,  2,  3,  4, 0,
0, 10, 20, 30, 1;
setBaseMultigrading(M, T);
ideal I = x, y, xy^5;
isHomogeneous(I);
intmat V = multiDeg(I); print(V);
module S = syz(I); print(S);
S = setModuleGrading(S, V);
getModuleGrading(S) == V;
vector v = getGradedGenerator(S, 1);
getModuleGrading(v) == V;
isHomogeneous(v);
print( multiDeg(v) );
isHomogeneous(S);
print( multiDeg(S) );
tst_status(1);$

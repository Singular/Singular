LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r = 0,(x,y,z),dp;
matrix C[3][3]=0,1,2,0,0,-1,0,0,0;
print(C);
matrix D[3][3]=0,1,2y,0,0,-2x+y+1;
print(D);
def S=nc_algebra(C,D);setring S; S;
def l=ncRelations(S);
print (l[1]);
print (l[2]);
tst_status(1);$

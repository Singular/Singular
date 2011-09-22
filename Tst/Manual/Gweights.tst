LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r = (0,q),(a,b,c,d),lp;
matrix C[4][4];
C[1,2]=q; C[1,3]=q; C[1,4]=1; C[2,3]=1; C[2,4]=q; C[3,4]=q;
matrix D[4][4];
D[1,4]=(q-1/q)*b*c;
def S = nc_algebra(C,D); setring S; S;
Gweights(S);
def D=fetch(r,D);
Gweights(D);
tst_status(1);$

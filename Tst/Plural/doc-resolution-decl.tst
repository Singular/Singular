LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),dp;
matrix D[3][3];
D[1,2]=z;
def S=ncalgebra(1,D); setring S; // it is Heisenberg algebra
ideal i=z2+z,x+y;
resolution re=nres(i,0);
re;
list l = re;
l;
tst_status(1);$

LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),dp;
matrix D[3][3];
D[1,2]=z;
def R=nc_algebra(1,D); // it is a Heisenberg algebra
setring R;
ideal i=z2+z,x+y;
resolution re=nres(i,0);
re;
list l = re;
l;
print(matrix(l[2]));
print(module(transpose(matrix(l[2]))*transpose(matrix(l[1]))));  // check that product is zero
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "central.lib";
ring R = 0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z;
def r = nc_algebra(1,D); setring r; // the Heisenberg algebra
poly f = x^2;
poly a = z; // 'z' is central => it lies in every centralizer!
poly b = y^2;
inCentralizer(a, f);
inCentralizer(b, f);
list  l = list(1, a);
inCentralizer(l, f);
ideal I = a, b;
inCentralizer(I, f);
printlevel = 2;
inCentralizer(a, f); // yes
inCentralizer(b, f); // no
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "perron.lib";
int p = 3;
ring AA = p,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z; D[1,3]=2*x; D[2,3]=-2*y;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2)
ideal I = x^p, y^p, z^p-z, 4*x*y+z^2-2*z; // the center
def RA = perron( I, p );
setring RA;
RA;
Relations; // it was exported from perron to be in the returned ring.
// perron can be also used in a commutative case, for example:
ring B = 0,(x,y,z),dp;
ideal J = xy+z2, z2+y2, x2y2-2xy3+y4;
def RB = perron(J);
setring RB;
Relations;
// one more test:
setring A;
map T=RA,I;
T(Relations);  // should be zero
tst_status(1);$

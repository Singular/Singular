LIB "tst.lib"; tst_init();
LIB "nctools.lib";
LIB "ncalg.lib";
ring r; // first classical example for modulo
ideal h1=x,y,z;    ideal h2=x;
module m=moduloSlim(h1,h2);
print(m);
// now, a noncommutative example
def A = makeUsl2(); setring A; // this algebra is U(sl_2)
ideal H2 = e2,f2,h2-1; H2 = twostd(H2);
print(matrix(H2)); // print H2 in a compact form
ideal H1 = std(e);
ideal T = moduloSlim(H1,H2);
T = std( NF(std(H2+T),H2) );
T;
// now, a matrix example:
ring r2 = 0,(x,d), (dp);
def R = nc_algebra(1,1); setring R;
matrix M[2][2] = d, 0, 0, d*(x*d);
matrix P[2][1] = (8x+7)*d+9x, (x2+1)*d + 5*x;
module X = moduloSlim(P,M);
print(X);
tst_status(1);$

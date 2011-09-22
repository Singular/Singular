LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z,w),dp;
intmat g[2][4]=
1,1,1,1,
0,1,3,4;
setBaseMultigrading(g);
module M = ideal(xw-yz, x2z-y3, xz2-y2w, yw2-z3);
intmat V[2][1]=
1,
0;
M = setModuleGrading(M, V);
def h = hilbertSeries(M); setring h;
factorize(numerator2);
factorize(denominator2);
kill g, h; setring r;
intmat g[2][4]=
1,2,3,4,
0,0,5,8;
setBaseMultigrading(g);
ideal I = x^2, y, z^3;
I = std(I);
def L = multiDegResolution(I, 0, 1);
for( int j = 1; j<=size(L); j++)
{
"----------------------------------- ", j, " -----------------------------";
L[j];
"Module Multigrading: "; print( getModuleGrading(L[j]) );
"Multigrading: "; print(multiDeg(L[j]));
}
multiDeg(I);
def h = hilbertSeries(I); setring h;
factorize(numerator2);
factorize(denominator2);
kill r, h, g, V;
////////////////////////////////////////////////
ring R = 0,(x,y,z),dp;
intmat W[2][3] =
1,1, 1,
0,0,-1;
setBaseMultigrading(W);
ideal I = x3y,yz2,y2z,z4;
def h = hilbertSeries(I); setring h;
factorize(numerator2);
factorize(denominator2);
kill R, W, h;
////////////////////////////////////////////////
ring R = 0,(x,y,z,a,b,c),dp;
intmat W[2][6] =
1,1, 1,1,1,1,
0,0,-1,0,0,0;
setBaseMultigrading(W);
ideal I = x3y,yz2,y2z,z4;
def h = hilbertSeries(I); setring h;
factorize(numerator2);
factorize(denominator2);
kill R, W, h;
////////////////////////////////////////////////
// This is example 5.3.9. from Robbianos book.
ring R = 0,(x,y,z,w),dp;
intmat W[1][4] =
1,1, 1,1;
setBaseMultigrading(W);
ideal I = z3,y3zw2,x2y4w2xyz2;
hilb(std(I));
def h = hilbertSeries(I); setring h;
numerator1;
denominator1;
factorize(numerator2);
factorize(denominator2);
kill h;
////////////////////////////////////////////////
setring R;
ideal I2 = x2,y2,z2; I2;
hilb(std(I2));
def h = hilbertSeries(I2); setring h;
numerator1;
denominator1;
kill h;
////////////////////////////////////////////////
setring R;
W = 2,2,2,2;
setBaseMultigrading(W);
getVariableWeights();
intvec w = 2,2,2,2;
hilb(std(I2), 1, w);
kill w;
def h = hilbertSeries(I2); setring h;
numerator1; denominator1;
kill h;
kill R, W;
////////////////////////////////////////////////
ring R = 0,(x),dp;
intmat W[1][1] =
1;
setBaseMultigrading(W);
ideal I;
I = 1; I;
hilb(std(I));
def h = hilbertSeries(I); setring h;
numerator1; denominator1;
kill h;
////////////////////////////////////////////////
setring R;
I = x; I;
hilb(std(I));
def h = hilbertSeries(I); setring h;
numerator1; denominator1;
kill h;
////////////////////////////////////////////////
setring R;
I = x^5; I;
hilb(std(I));
hilb(std(I), 1);
def h = hilbertSeries(I); setring h;
numerator1; denominator1;
kill h;
////////////////////////////////////////////////
setring R;
I = x^10; I;
hilb(std(I));
def h = hilbertSeries(I); setring h;
numerator1; denominator1;
kill h;
////////////////////////////////////////////////
setring R;
module M = 1;
M = setModuleGrading(M, W);
hilb(std(M));
def h = hilbertSeries(M); setring h;
numerator1; denominator1;
kill h;
////////////////////////////////////////////////
setring R;
kill M; module M = x^5*gen(1);
//  intmat V[1][3] = 0; // TODO: this would lead to a wrong result!!!?
intmat V[1][1] = 0; // all gen(i) of degree 0!
M = setModuleGrading(M, V);
hilb(std(M));
def h = hilbertSeries(M); setring h;
numerator1; denominator1;
kill h;
////////////////////////////////////////////////
setring R;
module N = x^5*gen(3);
kill V;
intmat V[1][3] = 0; // all gen(i) of degree 0!
N = setModuleGrading(N, V);
hilb(std(N));
def h = hilbertSeries(N); setring h;
numerator1; denominator1;
kill h;
////////////////////////////////////////////////
setring R;
module S = M + N;
S = setModuleGrading(S, V);
hilb(std(S));
def h = hilbertSeries(S); setring h;
numerator1; denominator1;
kill h;
kill V;
kill R, W;
tst_status(1);$

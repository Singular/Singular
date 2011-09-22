LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z,w),dp;
intmat M[2][4]=
1,1,1,1,
0,1,3,4;
setBaseMultigrading(M);
module m= ideal(  xw-yz, x2z-y3, xz2-y2w, yw2-z3);
isHomogeneous(ideal(  xw-yz, x2z-y3, xz2-y2w, yw2-z3), "checkGens");
ideal A = xw-yz, x2z-y3, xz2-y2w, yw2-z3;
int j;
for(j=1; j<=ncols(A); j++)
{
multiDegPartition(A[j]);
}
intmat v[2][1]=
1,
0;
m = setModuleGrading(m, v);
// Let's compute Syzygy!
def S = multiDegSyzygy(m); S;
"Module Units Multigrading: "; print( getModuleGrading(S) );
"Multidegrees: "; print(multiDeg(S));
/////////////////////////////////////////////////////////////////////////////
S = multiDegGroebner(S); S;
"Module Units Multigrading: "; print( getModuleGrading(S) );
"Multidegrees: "; print(multiDeg(S));
/////////////////////////////////////////////////////////////////////////////
def L = multiDegResolution(m, 0, 1);
for( j =1; j<=size(L); j++)
{
"----------------------------------- ", j, " -----------------------------";
L[j];
"Module Multigrading: "; print( getModuleGrading(L[j]) );
"Multigrading: "; print(multiDeg(L[j]));
}
/////////////////////////////////////////////////////////////////////////////
L = multiDegResolution(maxideal(1), 0, 1);
for( j =1; j<=size(L); j++)
{
"----------------------------------- ", j, " -----------------------------";
L[j];
"Module Multigrading: "; print( getModuleGrading(L[j]) );
"Multigrading: "; print(multiDeg(L[j]));
}
kill v;
def h = hilbertSeries(m);
setring h;
numerator1;
factorize(numerator1);
denominator1;
factorize(denominator1);
numerator2;
factorize(numerator2);
denominator2;
factorize(denominator2);
tst_status(1);$

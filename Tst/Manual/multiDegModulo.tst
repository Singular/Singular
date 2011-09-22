LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z),dp;
intmat MM[2][3]=
-1,1,1,
0,1,3;
setBaseMultigrading(MM);
ideal h1 = x, y, z;
ideal h2 = x;
"Multidegrees: "; print(multiDeg(h1));
// Let's compute modulo(h1, h2):
def K = multiDegModulo(h1, h2); K;
"Module Units Multigrading: "; print( getModuleGrading(K) );
"Multidegrees: "; print(multiDeg(K));
isHomogeneous(K);
tst_status(1);$

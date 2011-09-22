LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z),dp;
//Grading and Torsion matrices:
intmat M[3][3] =
1,0,0,
0,1,0,
0,0,1;
intmat T[3][1] =
1,2,3;
setBaseMultigrading(M,T);
attrib(r);
poly f = x-yz;
multiDegPartition(f);
print(multiDeg(_));
isHomogeneous(f);   // f: is not homogeneous
poly g = 1-xy2z3;
isHomogeneous(g); // g: is homogeneous
multiDegPartition(g);
kill T;
/////////////////////////////////////////////////////////
// new Torsion matrix:
intmat T[3][4] =
3,3,3,3,
2,1,3,0,
1,2,0,3;
setBaseMultigrading(M,T);
f;
isHomogeneous(f);
multiDegPartition(f);
// ---------------------
g;
isHomogeneous(g);
multiDegPartition(g);
kill r, T, M;
ring R = 0, (x,y,z), dp;
intmat A[2][3] =
0,0,1,
3,2,1;
intmat T[2][1] =
-1,
4;
setBaseMultigrading(A, T);
isHomogeneous(ideal(x2 - y3 -xy +z, x*y-z, x^3 - y^2*z + x^2 -y^3)); // 1
isHomogeneous(ideal(x2 - y3 -xy +z, x*y-z, x^3 - y^2*z + x^2 -y^3), "checkGens");
isHomogeneous(ideal(x+y, x2 - y2)); // 0
// Degree partition:
multiDegPartition(x2 - y3 -xy +z);
multiDegPartition(x3 -y2z + x2 -y3 + z + 1);
module N = gen(1) + (x+y) * gen(2), z*gen(3);
intmat V[2][3] = 0; // 1, 2, 3,  4, 5, 6; //  column-wise weights of components!!??
vector v1, v2;
v1 = setModuleGrading(N[1], V); v1;
multiDegPartition(v1);
print( multiDeg(_) );
v2 = setModuleGrading(N[2], V); v2;
multiDegPartition(v2);
print( multiDeg(_) );
N = setModuleGrading(N, V);
isHomogeneous(N);
print( multiDeg(N) );
///////////////////////////////////////
V =
1, 2, 3,
4, 5, 6;
v1 = setModuleGrading(N[1], V); v1;
multiDegPartition(v1);
print( multiDeg(_) );
v2 = setModuleGrading(N[2], V); v2;
multiDegPartition(v2);
print( multiDeg(_) );
N = setModuleGrading(N, V);
isHomogeneous(N);
print( multiDeg(N) );
///////////////////////////////////////
V =
0, 0, 0,
4, 1, 0;
N = gen(1) + x * gen(2), z*gen(3);
N = setModuleGrading(N, V); print(N);
isHomogeneous(N);
print( multiDeg(N) );
v1 = getGradedGenerator(N,1); print(v1);
multiDegPartition(v1);
print( multiDeg(_) );
N = setModuleGrading(N, V); print(N);
isHomogeneous(N);
print( multiDeg(N) );
tst_status(1);$

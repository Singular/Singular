LIB "tst.lib"; tst_init();
LIB "involut.lib";
def a = makeWeyl(1);
setring a; // this algebra is a first Weyl algebra
a;
def X = findAuto(2);  // in contrast to findInvo look for automorphisms
setring X; // ring with new variables - unknown coefficients
X;
size(L); // we have (size(L)) families in the answer
// look at matrices, defining linear automorphisms:
print(L[1][2]);  // a first one: we see it is the identity
print(L[2][2]);  // and a second possible matrix; it is diagonal
// L; // we can take a look on the whole list, too
idJ;
kill X; kill a;
//----------- find all the linear automorphisms --------------------
//----------- use the call findAuto(0)          --------------------
ring R = 0,(x,s),dp;
def r = nc_algebra(1,s); setring r; // the shift algebra
s*x; // the only relation in the algebra is:
def Y = findAuto(0);
setring Y;
size(L); // here, we have 1 parametrized family
print(L[1][2]); // here, @p is a nonzero parameter
det(L[1][2]-@p);  // check whether determinante is zero
tst_status(1);$

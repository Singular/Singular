LIB "tst.lib"; tst_init();
LIB "involut.lib";
def a = makeWeyl(1);
setring a; // this algebra is a first Weyl algebra
a;
def X = findInvo();
setring X; // ring with new variables, corr. to unknown coefficients
X;
L;
// look at the matrix in the new variables, defining the linear involution
print(L[1][2]);
L[1][1];  // where new variables obey these relations
idJ;
tst_status(1);$

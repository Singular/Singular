LIB "tst.lib"; tst_init();
LIB "involut.lib";
def a = makeWeyl(1);
setring a; // this algebra is a first Weyl algebra
a;
def X = findInvoDiag();
setring X; // ring with new variables, corresponding to unknown coefficients
X;
// print matrices, defining linear involutions
print(L[1][2]);  // a first matrix: we see it is constant
print(L[2][2]);  // and a second possible matrix; it is constant too
L; // let us take a look on the whole list
idJ;
tst_status(1);$

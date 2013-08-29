LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def R = makeQso3(3);
setring R; R;
option(redSB);
// we wish to have completely reduced bases:
option(redTail);
module tst;
ideal J = x3+x,x*y*z;
option(); print(syz(J));
ideal K = x+y+z,y+z,z;
option(); module S = syz(K);
print(S);
tst = module(transpose(S)*transpose(K));
// check the property of a syzygy module (tst==0):
size(tst);
 // now compute the Groebner basis of K ...
option(); K = std(K);
// ... print a matrix presentation of K ...
print(matrix(K));
option(); S = syz(K); // ... and its syzygy module
print(S);
tst = module(transpose(S)*transpose(K));
// check the property of a syzygy module (tst==0):
size(tst);
// but the "commutative" syzygy property does not hold
size(module(matrix(K)*matrix(S)));
tst_status(1);$

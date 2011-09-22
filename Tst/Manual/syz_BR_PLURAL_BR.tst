LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def R = makeQso3(3);
setring R;
option(redSB);
// we wish to have completely reduced bases:
option(redTail);
ideal tst;
ideal J = x3+x,x*y*z;
print(syz(J));
ideal K = x+y+z,y+z,z;
module S = syz(K);
print(S);
tst = ideal(transpose(S)*transpose(K));
// check the property of a syzygy module (tst==0):
size(tst);
 // now compute the Groebner basis of K ...
K = std(K);
// ... print a matrix presentation of K ...
print(matrix(K));
S = syz(K); // ... and its syzygy module
print(S);
tst = ideal(transpose(S)*transpose(K));
// check the property of a syzygy module (tst==0):
size(tst);
// but the "commutative" syzygy property does not hold
size(ideal(matrix(K)*matrix(S)));
tst_status(1);$

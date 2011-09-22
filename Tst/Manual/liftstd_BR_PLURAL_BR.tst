LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def A = makeUsl2();
setring A;  // this algebra is U(sl_2)
ideal i = e2,f;
option(redSB);
option(redTail);
matrix T;
ideal j = liftstd(i,T);
// the Groebner basis in a compact form:
print(matrix(j));
print(T);  // the transformation matrix
ideal tj = ideal(transpose(T)*transpose(matrix(i)));
size(ideal(matrix(j)-matrix(tj))); // test for 0
module S; ideal k = liftstd(i,T,S); // the third argument
S = std(S); print(S); // the syzygy module
tst_status(1);$

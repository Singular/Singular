LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def A = makeUsl2();
setring A; // this algebra is U(sl_2)
option(redSB);
option(redTail);
ideal i = e,f,h;
i = std(i);
resolution M=mres(i,0);
M;
list l = M; l;
// see the exactness at this point
size(ideal(transpose(l[2])*transpose(l[1])));
print(matrix(M[3]));
// see the exactness at this point
size(ideal(transpose(l[3])*transpose(l[2])));
tst_status(1);$

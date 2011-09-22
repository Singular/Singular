LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def A = makeUsl2();
setring A; // this algebra is U(sl_2)
option(redSB);
option(redTail);
ideal H2 = e2,f2,h2-1;
H2 = twostd(H2);
print(matrix(H2)); // print H2 in a compact form
ideal H1 = std(e);
ideal T = modulo(H1,H2);
T = NF(std(H2+T),H2);
T = std(T);
T;
tst_status(1);$

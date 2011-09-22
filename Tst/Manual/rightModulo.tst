LIB "tst.lib"; tst_init();
LIB "nctools.lib";
LIB "ncalg.lib";
def A = makeUsl(2);
setring A;
option(redSB);
option(redTail);
ideal I = e2,f2,h2-1;
I = twostd(I);
print(matrix(I));
ideal E  = std(e);
ideal TL = e,h-1; // the result of left modulo
TL;
ideal T = rightModulo(E,I);
T = rightStd(T+I);
T = rightStd(rightNF(T,I)); // make the output canonic
T;
tst_status(1);$

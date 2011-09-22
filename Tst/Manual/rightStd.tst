LIB "tst.lib"; tst_init();
LIB "nctools.lib";
LIB "ncalg.lib";
def A = makeUsl(2);
setring A;
ideal I = e2,f;
option(redSB);
option(redTail);
ideal LI = std(I);
LI;
ideal RI = rightStd(I);
RI;
tst_status(1);$

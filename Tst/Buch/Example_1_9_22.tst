LIB "tst.lib";
tst_init();

=============================  example 1.9.22  ==============================

LIB "ncalg.lib";
def A = makeUsl(2);
setring A;
option(redSB);  
option(redTail); // we wish to compute reduced bases
ideal I = e2,f;
ideal LI = std(I);
print(matrix(LI));  // a compact form of an ideal
ideal RI = rightStd(I);
print(matrix(RI));

tst_status(1);$


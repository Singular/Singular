LIB "tst.lib";
tst_init();

// ===========================  example 1.9.23  ===============================
LIB "ncalg.lib";
def A=makeUsl(2);
setring A;

option(redSB);
option(redTail);

ideal I = e2,f;
ideal LI = std(I);
print(matrix(LI));  // a compact form of an ideal
ideal TI = twostd(I);
print(matrix(TI));

tst_status(1);$


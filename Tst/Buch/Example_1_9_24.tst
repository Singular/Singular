LIB "tst.lib";
tst_init();

=============================  example 1.9.24  ==============================

LIB "ncalg.lib";
def A = makeUsl2();
setring A;
ideal T = 4*e*f+h^2-2*h; // central element in U(sl_2)
T = twostd(T);     
T;
qring Q = twostd(T);
ideal I = e2,f;
ideal LI = std(I);
LI;

tst_status(1);$


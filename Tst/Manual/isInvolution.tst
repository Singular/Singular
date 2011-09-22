LIB "tst.lib"; tst_init();
LIB "involut.lib";
def A = makeUsl(2); setring A;
map I = A,-e,-f,-h; //correct antiauto involution
isInvolution(I);
map J = A,3*e,1/3*f,-h; // antiauto but not involution
isInvolution(J);
map K = A,f,e,-h; // not antiauto
isInvolution(K);
tst_status(1);$

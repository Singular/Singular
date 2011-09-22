LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r=(0,a,b),(x(1..3)),dp;
matrix S[3][3];
S[2,3]=a*x(1); S[3,2]=-b*x(1);
def A=findimAlgebra(S); setring A;
fdQuot = twostd(fdQuot);
qring Qr = fdQuot;
Qr;
tst_status(1);$

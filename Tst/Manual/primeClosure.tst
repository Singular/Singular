LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring R=0,(x,y),dp;
ideal I=x4,y4;
def K=ReesAlgebra(I)[1];        // K contains ker such that K/ker=R[It]
list L=primeClosure(K);
def R(1)=L[1];                  // L[4] contains ker, L[4]/ker is the
def R(4)=L[4];                  // integral closure of L[1]/ker
setring R(1);
R(1);
ker;
setring R(4);
R(4);
ker;
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "qmatrix.lib";
def r = quantMat(2); // generate O_q(M_2) at q generic
setring r;   r;
kill r;
def r = quantMat(2,5); // generate O_q(M_2) at q^5=1
setring r;   r;
tst_status(1);$

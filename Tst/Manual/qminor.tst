LIB "tst.lib"; tst_init();
LIB "qmatrix.lib";
def r = quantMat(3); // let r be a quantum matrix of order 3
setring r;
intvec u = 1,2;
intvec v = 2,3;
intvec w = 1,2,3;
qminor(w,w,3);
qminor(u,v,3);
qminor(v,u,3);
qminor(u,u,3);
tst_status(1);$

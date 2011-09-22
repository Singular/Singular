LIB "tst.lib"; tst_init();
LIB "reesclos.lib";
ring R=0,(x,y),dp;
ideal I = x2,xy4,y5;
list J = normalI(I);
I;
J;                             // J[1] is the integral closure of I
tst_status(1);$

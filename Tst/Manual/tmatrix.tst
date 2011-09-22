LIB "tst.lib"; tst_init();
LIB "gmssing.lib";
ring R=0,(x,y),ds;
poly t=x5+x2y2+y5;
list l=tmatrix(t);
print(l[1]);
print(l[2]);
print(l[3]);
print(l[4]);
tst_status(1);$

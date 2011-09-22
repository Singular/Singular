LIB "tst.lib"; tst_init();
LIB "mondromy.lib";
ring R=0,(x,y),ds;
poly f=x2y2+x6+y6;
matrix M=monodromyB(f);
print(M);
tst_status(1);$

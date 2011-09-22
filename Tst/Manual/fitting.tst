LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring R=0,x(0..4),dp;
matrix M[2][4]=x(0),x(1),x(2),x(3),x(1),x(2),x(3),x(4);
print(M);
fitting(M,-1);
fitting(M,0);
fitting(M,1);
fitting(M,2);
tst_status(1);$

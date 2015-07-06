LIB "tst.lib"; tst_init();
ring R=integer,(t,a,b,c,d),dp;
ideal I = 9, 3*b, 3*a, 3*t-3, t*c+c;
std(I);
tst_status(1);$

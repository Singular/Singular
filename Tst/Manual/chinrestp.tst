LIB "tst.lib"; tst_init();
LIB "hyperel.lib";
ring R=7,x,dp;
list b=3x-4, -3x2+1, 1, 4;
list moduli=(x-2)^2, (x-5)^3, x-1, x-6;
chinrestp(b,moduli);
tst_status(1);$

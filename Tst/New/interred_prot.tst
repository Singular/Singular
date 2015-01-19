option();

LIB "tst.lib"; tst_init();

option();

ring s12=32003,(x,y,z,w),dp;
ideal i0=x2-z10-z20,xy3-z10-z30,y6-xy3w40;
option(redSB);
option(prot);
test(23+32);

option();
interred(i0^3);
option();

tst_status(1);$

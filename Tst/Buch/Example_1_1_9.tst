
LIB "tst.lib";
tst_init();

ring A = 0,(x,y,z),dp;
poly f = x3+y2+z2;        
f*f-f;

tst_status(1);$

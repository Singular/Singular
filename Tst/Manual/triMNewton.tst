LIB "tst.lib"; tst_init();
LIB "ntsolve.lib";
ring r = (real,30),(z,y,x),(lp);
ideal i = x^2-1,y^2+x4-3,z2-y4+x-1;
ideal a = 2,3,4;
intvec e = 20,10;
ideal l = triMNewton(i,a,e);
l;
tst_status(1);$

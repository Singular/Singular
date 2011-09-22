LIB "tst.lib"; tst_init();
LIB "dmodvar.lib";
ring R = 0,(x,y,z),dp;
ideal F = x^2+y^3, z;
list L = bfctVarIn(F);
L;
tst_status(1);$

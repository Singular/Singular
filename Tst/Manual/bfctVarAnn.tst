LIB "tst.lib"; tst_init();
LIB "dmodvar.lib";
ring R = 0,(x,y,z),Dp;
ideal F = x^2+y^3, z;
bfctVarAnn(F);
tst_status(1);$

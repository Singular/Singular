LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ux","Ut","U";
list P="a";
list V="t","x";
setinitials(V,D,P);
laxfrT(Ux,U,x);
tst_status(1);$

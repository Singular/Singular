LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Uxx","Ux","Utt","Ut","U";
list P="lambda";
list V="t","x";
setinitials(V,D,P);
midpoint(Ux,U,x);
tst_status(1);$

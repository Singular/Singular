LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Uxx","Ux","Utt","Ut","U";
list P="lambda";
list V="t","x";
setinitials(V,D,P);
central2nd(Uxx,U,x);
central2nd(Utt,U,t);
tst_status(1);$

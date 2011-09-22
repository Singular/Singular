LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ux","Ut","U";
list P="a";
list V="t","x";
setinitials(V,D,P);
scheme(u(Ut)+a*u(Ux),central1st(Ux,U,x),backward(Ut,U,t));
errormap(_);
tst_status(1);$

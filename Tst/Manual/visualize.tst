LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ux","Ut","U";
list P="a";
list V="t","x";
setinitials(V,D,P);
scheme(u(Ut)+a*u(Ux),trapezoid(Ux,U,x),backward(Ut,U,t));
visualize(_);
tst_status(1);$

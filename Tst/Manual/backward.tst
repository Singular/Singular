LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);
backward(Ux,U,x);
backward(Uy,U,y);
backward(Ut,U,t);
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);
forward(Ux,U,x);
forward(Uy,U,y);
forward(Ut,U,t);
tst_status(1);$

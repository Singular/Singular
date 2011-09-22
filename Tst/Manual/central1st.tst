LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);
central1st(Ux,U,x);
central1st(Uy,U,y);
tst_status(1);$

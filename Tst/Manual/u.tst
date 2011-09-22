LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ux","Uy","Ut","U";
list P="a","b";
list V="t","x","y";
setinitials(V,D,P);
u(Ux);
u(Ux,2,3,7);
u(Uy)+u(Ut)-u(Ux);
u(U)*234-dx*dt*dy*3*u(Uy);
tst_status(1);$

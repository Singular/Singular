LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);
list Mat=unitmat(2),unitmat(2);
list Apr=forward(Ut,U,t),forward(Ux,U,x);
matrixsystem(Mat,Apr);
timestep(_);
tst_status(1);$

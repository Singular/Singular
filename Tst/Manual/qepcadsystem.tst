LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);
matrix M[2][2]=0,-a,-a,0;
list Mat=unitmat(2),M;
list Appr=forward(Ut,U,t),forward(Ux,U,x);
//matrixsystem(Mat,Appr);
//timestep(_);
fouriersystem(Mat,Appr);
qepcadsystem(_[2]);
tst_status(1);$

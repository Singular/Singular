LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ut","Ux","Uy","U";
list V="t","x","y";
list P="a","b";
setinitials(V,D,P);
matrix M[2][2]=0,-a,-a,0;
list Mat=unitmat(2),M;
list Appr=forward(Ut,U,t),trapezoid(Ux,U,x);
def s=fouriersystem(Mat,Appr);s;
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "spcurve.lib";
ring r=0,(x,y,z),ds;
matrix M[3][2]=z-x^7,0,y^2,z,x^9,y;
def rneu=KSpencerKernel(M,"ar");
setring rneu;
basering;
print(KS);
tst_status(1);$

LIB "tst.lib"; tst_init();
LIB "arcpoint.lib";
ring r=0,(x,y,z),dp;
poly f=z4+y3-x2;
def R=nashmult(f,2);
setring R;
allsteps;
tst_status(1);$

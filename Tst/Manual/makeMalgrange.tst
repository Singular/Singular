LIB "tst.lib"; tst_init();
LIB "dmodvar.lib";
ring R = 0,(x,y,z),Dp;
ideal I = x^2+y^3, z;
def W = makeMalgrange(I);
setring W;
W;
IF;
tst_status(1);$

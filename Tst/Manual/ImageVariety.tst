LIB "tst.lib"; tst_init();
LIB "rinvar.lib";
ring B   = 0,(x,y),dp;
ideal I  = x4 - y4;
ideal F  = x2, y2, x*y;
def R = ImageVariety(I, F);
setring R;
imageid;
tst_status(1);$

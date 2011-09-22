LIB "tst.lib"; tst_init();
LIB "ncdecomp.lib";
option(returnSB);
def a = makeUsl2();
setring a;
ideal I = e3,f3,h3-4*h;
I = std(I);
poly C=4*e*f+h^2-2*h;
ideal G = C*(C-8);
ideal R = CentralSaturation(I,G);
R=std(R);
vdim(R);
R;
tst_status(1);$

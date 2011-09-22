LIB "tst.lib"; tst_init();
LIB "ncdecomp.lib";
option(returnSB);
def a = makeUsl2();
setring a;
ideal I = e3,f3,h3-4*h;
I = std(I);
poly C=4*e*f+h^2-2*h;  // C in Z(U(sl2)), the central element
ideal G = (C-8)*(C-24);  // G normal factor in Z(U(sl2)) as an ideal in the center
ideal R = CentralQuot(I,G);  // same as I:G
R;
tst_status(1);$

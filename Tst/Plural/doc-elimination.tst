// simple elimination tests
LIB "tst.lib";
tst_init();
ring r=0,(e,f,h,a),Dp;
matrix d[4][4];
d[1,2]=-h;
d[1,3]=2*e;
d[2,3]=-2*f;
def S=nc_algebra(1,d);setring S;
// it is U(sl_2)+K[a]
option(redSB);
option(redTail);
poly p=4*e*f+h^2-2*h-a;
// this is central element with parameter
ideal I=e^3,f^3, h^3-4*h,p;
ideal J=eliminate(I,efh);
// this will give us the central character of I
J;
eliminate(I,h); // [e,f] is not the adm. subalgebra!
eliminate(I,a);
tst_status(1);$

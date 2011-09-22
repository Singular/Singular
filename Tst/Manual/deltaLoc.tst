LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring r=0,(x,y),dp;
poly f=(x2+y^2-1)^3 +27x2y2;
ideal I=f,jacob(f);
I=std(I);
list qr=minAssGTZ(I);
size(qr);
// each component of the singular locus either describes a cusp or a pair
// of conjugated nodes:
deltaLoc(f,qr[1]);
deltaLoc(f,qr[2]);
deltaLoc(f,qr[3]);
deltaLoc(f,qr[4]);
deltaLoc(f,qr[5]);
deltaLoc(f,qr[6]);
tst_status(1);$

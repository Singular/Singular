LIB "tst.lib";
tst_init();

// module std over qring with ring-coeffs
ring r1 = integer,(x,y,z),dp;
ideal i = x4+y2+z2;
qring q = i;
ideal s = 2z,-4z2,-4yz,-4xz,-2x2z-2yz-2z2,-x4-y2-z2;
syz(s); 
ring r2=integer,(x, y),dp;
ideal I=81*y^6-162*y^5;
qring qr=I;
ideal s=6;
syz(s);

tst_status(1);$

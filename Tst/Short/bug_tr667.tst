LIB "tst.lib";
tst_init();

// minAssChar (tr.667, tr.669)
LIB"primdec.lib";
ring rng = 0,(y,t,o,u,j,x,s,z),dp;
ideal I = -17*y*j^2*x*z-26*t*s+17*j,24*y^2*t*o^2+12*y,-10*y-7*t*u^2*j*x*z-25*j;
list L2 = minAssChar(I);
L2;  // size should be 4, not 5
minAssChar(L2[2]); //  size is 2, should be 1
minAssChar(L2[3]); //  size is 2, should be 1
list L1 = minAssGTZ(I);
size(L1);

ring r = (7,vv),(z,t,c,i),dp;
minpoly = (vv^2+vv+3);
ideal I =
(3*vv-2)*z*i+(vv-3),(3*vv-2)*c^3+3*c-3,z*t^2+3*t*i+(3*vv)*t-i;

primdecGTZ(I);
primdecSY(I);

tst_status(1);$

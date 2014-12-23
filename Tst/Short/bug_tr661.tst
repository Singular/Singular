LIB "tst.lib";
tst_init();

// incorrect radical entry in a primdecSY
LIB"primdec.lib";
ring rng = (0),(m,p,w,o,j,d,n,z),dp;

ideal I = -2*j+19*o^2,
18*w*z+19*d,
-13*p*d*z-16*o,
-2*j*n^2+9*n,
23*m-8*j,
-17*m*n+17*m*o,
-18*n*z+3*j,
23*m*p*n-30*w*n,
-6*m*w,
23*j*d*z+4*d*z,
16*m*w+14*j,
-20*p*j*n+27*z^2,
23*p*w+14*m*d,
7*m*j+29*o*d*n;

list  L = primdecSY(I);
ideal pc2 = L[2][1];
pc2;
ideal rpc2 = L[2][2]; // incorrect!!
rpc2;

radical(L[2][1]);

idealsEqual( radical(pc2), rpc2 ); // = 0!
///////////////////////////////////////////
// same with groebner(I):
L = primdecSY (groebner(I) );
pc2 = L[2][1];
rpc2 = L[2][2];
rpc2;
radical(L[2][1]);
idealsEqual( radical(pc2), rpc2 ); //ok!

tst_status(1);$

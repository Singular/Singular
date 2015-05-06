LIB "tst.lib";
tst_init();

LIB "primdec.lib";
system("random",1580702082);
ring rng = (5,vv),(x,y,z),dp;
minpoly = vv^2-2;
ideal I = (-2*vv)*x*z+(-vv)*y^3,x*y*z-2*x*y+2*y^3,-x+z^2+(-2*vv);
list L1 =  primdecGTZ (I);
list L2 = primdecSY (I);
L1;
L2;

testPrimary(L1 , I) ;
testPrimary(L2 , I) ;

// here is the issue:
idealsEqual(L1[4][2],L2[4][2]); //ok
idealsEqual(L1[4][1],L2[4][1]); //=0

tst_status(1);$

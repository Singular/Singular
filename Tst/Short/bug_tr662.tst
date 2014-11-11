LIB "tst.lib";
tst_init();

// primary decomposition with wromng component
LIB"primdec.lib";
option(redTail);
option(redSB);
ring rng = (0),(m,g,r,w,j,a,i,q),dp;
ideal I = -3*g*i+30*m^2,
16*w*q-7*r*a,
-17*g*i*q-7*m*r*q,
-22*m*g+11*w^2;

list L = primdecGTZ (std(I));

size(L); // should be 7
L[4][1];
isSubModule(I, L[4][1] );

testPrimary( L , I ) ;

list L2 = primdecSY (std(I));
size(L2); // should be 7
testPrimary( L2 , I ) ;

tst_status(1);$

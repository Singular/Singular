LIB "tst.lib";
tst_init();

LIB "primdec.lib";
ring rng = 2,(x,y,z),dp;
ideal I = x^2*y+x*z^2+1, x^2*z+z^2, z^3+1;
system("random",1426357199);
def L2 = primdecGTZ(I);

//here is the problem
idealsEqual( radical( L2[2][1] ),  L2[2][2]  ); // 0 
L2[2][2] ;
radical( L2[2][1] );
std(L2[2][2]);
std(radical( L2[2][1] ));

tst_status(1);$

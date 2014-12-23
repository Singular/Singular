LIB "tst.lib";
tst_init();

LIB("primdecint.lib");

ring rng = integer,(x,y),dp;
ideal I = 1;
def pdec = primdecZ(I);
ASSUME(0, size(pdec)==0);

I=0;
pdec = primdecZ(I);
ASSUME(0, size( pdec ) ==1  );
ASSUME(0, size( pdec[1] ) == 2 ) ;
ASSUME(0, size( pdec[1][1] )==0 );
ASSUME(0, size( pdec[1][2] )==0 );


I = 8*x;
pdec = primdecZ(I);


tst_status(1); $


LIB "tst.lib";
tst_init();

LIB("ehv.lib");

ring  r = 0,(x,y),dp;
ideal J = x;
ideal I = x, 0 , y;
ASSUME( 0, isSubset( I, std(J) ) == isSubset( std(I), std(J) ) );
J = x+3,5;
// J is not standard basis:
attrib(J,"isSB");

//no warning should be printed , since isSubset computes standard basis for J before calling reduce!
isSubset( I, J );

tst_status(1);$;



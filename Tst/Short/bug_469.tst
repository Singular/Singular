LIB "tst.lib";
tst_init();

LIB("normal.lib");

ring rng = 0,( x, y, z),dp;

ideal EI =  -5*x^2*y, -9*z^3+5;
genus(EI);
genus(EI,"nor");
genus(EI,"pri");

ASSUME(0, genus(EI)       == genus(EI,"pri") );
ASSUME(0, genus(EI,"nor") == genus(EI,"pri") );

EI = z^2, x*y;
genus(EI);
genus(EI,"nor");
genus(EI,"pri");

ASSUME(0, genus(EI)       == genus(EI,"pri") );
ASSUME(0, genus(EI,"nor") == genus(EI,"pri") );


tst_status(1); $


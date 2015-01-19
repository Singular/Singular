LIB "tst.lib";
tst_init();

// genus need a reduced cure:
LIB "primdec.lib";

ring rng = 0,(x,y),dp;

ideal IR = y^2+2, x*y+1, 2*x^2+1;
ideal IRstd = std(IR);
IRstd; // = 2*x-y,y^2+2

equidimMax(IR) ;;             // = 2*x-y,y^2+2
std( equidimMaxEHV(IRstd) );  // = 2*x-y,y^2+2
equidimMaxEHV(IR) ;           // should be equal
// -------------------------------------------------
LIB"normal.lib";
ring r2 = 0,( xw, xd, xr),dp;

ideal EI =  -5*xw^2*xd, -9*xr^3+5;
genus(EI);
genus(EI,"nor");
genus(EI,"pri");

// -------------------------------------------------
system("random", 523454);
ring r3 = 23,(x, y, z), dp;
ideal I = 9*x*y^2-11*x*z^2, 11*x*y*z^2+4*x^2*z, -3*y^5+x*z-10*y*z;

ideal RI = std(radical(I));

list L1 = primdecGTZ(RI);
L1;


tst_status(1);$

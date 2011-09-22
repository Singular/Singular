LIB "tst.lib"; tst_init();
LIB "grobcov.lib";
ring R=(0,a,b,c),(x,y),dp;
"Divisor=";
poly f=(ab-ac)*xy+(ab)*x+(5c);
"Dividends=";
ideal F=ax+b,cy+a;
"(Remainder, quotients, factor)=";
def r=pdivi(f,F);
r;
"Verifying the division: r[3]*f-(r[2][1]*F[1]+r[2][2]*F[2])-r[1] =";
r[3]*f-(r[2][1]*F[1]+r[2][2]*F[2])-r[1];
tst_status(1);$

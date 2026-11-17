LIB "tst.lib";
tst_init();

LIB "grobcov.lib";

// PtoCrep must set the options needed for canonical output itself and
// restore the complete caller option vector.
ring A=(0,a,b),(x),dp;
ideal Pideal=a*b;
ideal Qideal=a,b-1;
list prep=Prep(Pideal,Qideal);

option(noredSB); option(noreturnSB);
intvec options_off=option(get);
list C0=PtoCrep(prep);
option(get)==options_off;

option(redSB); option(returnSB);
intvec options_on=option(get);
list C1=PtoCrep(prep);
option(get)==options_on;
string(C0)==string(C1);

// Restoring options in static helpers must not change the public cover.
ring R=(0,a,b,l2,l3),(c3,s3,c1,s1),dp;
ideal F=a-l3*c3-l2*c1,
        b-l3*s3-l2*s1,
        c1^2+s1^2-1,
        c3^2+s3^2-1;

option(noredSB); option(noreturnSB);
options_off=option(get);
list G0=grobcov(F,"rep",1);
option(get)==options_off;

option(redSB); option(returnSB);
options_on=option(get);
list G1=grobcov(F,"rep",1);
option(get)==options_on;
string(G0)==string(G1);

tst_status(1);$

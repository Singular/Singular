LIB "tst.lib";
tst_init();

LIB "grobcov.lib";
ring R=(0,a,b),(x),dp;
ideal F=a*x-b;

// cgsdr must preserve the complete caller option vector and its result
// must not depend on the caller's redSB and returnSB settings (#1417).
option(noredSB); option(noreturnSB);
intvec options_off=option(get);
list L0=cgsdr(F);
option(get)==options_off;
size(L0)==3;

option(redSB); option(returnSB);
intvec options_on=option(get);
list L1=cgsdr(F);
option(get)==options_on;
string(L0)==string(L1);

tst_status(1);$

LIB "tst.lib";
tst_init();
tst_ignore("not provided yet");
LIB "primdecint.lib";
///////////////////////////////////////////////////////

proc testRadicalZ()
{
    ring rng=integer,( a,b,y(1),d ),dp;
    ideal j = 2*d,d^2,y(1)^2 - y(1)*d, b*y(1) - 2*d,  a*b + y(1)*d;
    def result = radicalZ(j);
    return();
}

// the test fails, if inside of radicalZ will be an infinite loop
watchdog(1,"testRadicalZ()");


tst_status(1);$


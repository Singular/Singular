LIB "tst.lib";
tst_init();

LIB("primdecint.lib");

ring rng=integer,( a,b,y(1),d ),dp;

proc testRadicalZ()
{
    ring rng=integer,( a,b,y(1),d ),dp;
    ideal j = 2*d,d^2,y(1)^2 - y(1)*d, b*y(1) - 2*d,  a*b + y(1)*d;
    def result = Primdecint::radicalZ(j);
}

proc testRadicalZ_2()
{
    ring rng = integer,(a,b,c),dp;   
    ideal i = a^2*b^4 + c^2-1,4*a^2*b^3; 
    def result = Primdecint::radicalZ(i);
    ideal expectedResult = 2*c^ 2-2, 2*a*b, a*b^2*c-a*b^ 2+c^2-1,a^2*b^3+a*b*c+a*b;
    ASSUME( 0, 0 == size(   reduce(result,std(expectedResult) )   ) );
    ASSUME( 0, 0 == size(   reduce(expectedResult,std(result) )   ) );
}

// the test fails, if inside of radicalZ will be an infinite loop
link l1 = "ssi:fork"; open(l1);

write( l1, quote(testRadicalZ_2()) );
waitfirst( list(l1), 2000); // waits 2 seconds.

if (not (status(l1,"read")=="ready")) { ERROR("testRadicalZ_2 failed!"); }
close(l1);
kill l1;

link l1 = "ssi:fork"; open(l1);

write( l1, quote( testRadicalZ() ) );
waitfirst( list(l1), 2000); // waits 2 seconds.

if (not (status(l1,"read")=="ready")) { ERROR("radicalZ() probably ended in an infinite loop!"); }
close(l1);
kill l1;

tst_status(1); $


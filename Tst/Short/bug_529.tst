LIB "tst.lib";
tst_init();

option(warn);
LIB("primdec.lib");

proc minAssPrimesW(def I)
{
   return(Primdec::minAssPrimes(I) ) ;
}

proc newDecompStepW(ideal i, list #)
{
   return(Primdec::newDecompStep(I,#) ) ;
}

ring rng =(0),(xc,xg,xq),(dp(3),C);
ideal I = xq,xg;
newDecompStepW(I);
minAssPrimesW(7); 

option(nowarn);
tst_status(1); $


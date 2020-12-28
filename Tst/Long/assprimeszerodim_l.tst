LIB "tst.lib";
tst_init();

LIB "assprimeszerodim.lib";

ring R1 = 0,x(1..6),dp;
ideal I = cyclic(6);
assPrimes(I, "GTZ");
system("random", 138256896);
assPrimes(I, "EHV");
system("random", 570594853);
assPrimes(I, "Monico");
system("random", 17715229);

// example from manual
ring R2 = 0,(a,b,c,d,e,f),dp;
ideal I = 2fb+2ec+d2+a2+a,
          2fc+2ed+2ba+b,
          2fd+e2+2ca+c+b2,
          2fe+2da+d+2cb,
          f2+2ea+e+2db+c2,
          2fa+f+2eb+2dc;
assPrimes(I, "GTZ");
system("--random", 12345);   // to get the same results on 32 and 64 bit
assPrimes(I, "EHV");
system("--random", 12345);   // to get the same results on 32 and 64 bit
assPrimes(I, "Monico");

tst_status(1);$

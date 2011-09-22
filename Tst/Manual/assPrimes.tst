LIB "tst.lib"; tst_init();
LIB "assprimeszerodim.lib";
ring R = 0,(a,b,c,d,e,f),dp;
ideal I =
2fb+2ec+d2+a2+a,
2fc+2ed+2ba+b,
2fd+e2+2ca+c+b2,
2fe+2da+d+2cb,
f2+2ea+e+2db+c2,
2fa+f+2eb+2dc;
assPrimes(I);
tst_status(1);$

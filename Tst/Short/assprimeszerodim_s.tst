LIB "tst.lib";
tst_init();

LIB "assprimeszerodim.lib";

// example from manual
ring R = 0, (x,y), dp;
ideal I = xy4-2xy2+x, x2-x, y4-2y2+1;
zeroRadical(I);

// more examples
ring R3 = 0,(x,y,z),dp;
ideal I = x2+xy2z-2xy+y4+y2+z2,
          -x3y2+xy2z+xyz3-2xy+y4,
          -2x2y+xy4+yz4-3;
assPrimes(I, "GTZ");
assPrimes(I, "EHV");
assPrimes(I, "Monico");

ring R4 = 0,(a,b,c,d,e,f,g,h,k,o),dp;
ideal I = o+1, k4+k, hk, h4+h, gk, gh, g3+h3+k3+1,
          fk, f4+f, eh, ef, f3h3+e3k3+e3+f3+h3+k3+1,
          e3g+f3g+g, e4+e, dh3+dk3+d, dg, df, de,
          d3+e3+f3+1, e2g2+d2h2+c, f2g2+d2k2+b,
          f2h2+e2k2+a;
assPrimes(I, "GTZ");
system("--random", 12345);   // to get the same results on 32 and 64 bit
assPrimes(I, "EHV");
system("--random", 12345);   // to get the same results on 32 and 64 bit
assPrimes(I, "Monico");

tst_status(1);$

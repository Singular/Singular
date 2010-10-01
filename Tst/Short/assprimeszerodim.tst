LIB "tst.lib";
tst_init();

LIB "assprimeszerodim.lib"

ring R1 = 0,x(1..6),dp;
ideal I = cyclic(6);
assPrimes(I,1);
assPrimes(I,1,4);
assPrimes(I,2);
assPrimes(I,2,4);
assPrimes(I,3);
assPrimes(I,3,4);

ring R2 = 0,(a,b,c,d,e,f),dp;
ideal I = 2fb+2ec+d2+a2+a,
          2fc+2ed+2ba+b,
          2fd+e2+2ca+c+b2,
          2fe+2da+d+2cb,
          f2+2ea+e+2db+c2,
          2fa+f+2eb+2dc;
assPrimes(I,1);
assPrimes(I,1,4);
assPrimes(I,2);
assPrimes(I,3);

ring R3 = 0,(x,y,z),dp;
ideal I = x2+xy2z-2xy+y4+y2+z2,
          -x3y2+xy2z+xyz3-2xy+y4,
          -2x2y+xy4+yz4-3;
assPrimes(I,1);
assPrimes(I,2);
assPrimes(I,2,4);
assPrimes(I,3);

ring R4 = 0,(a,b,c,d,e,f,g,h,k,o),dp;
ideal I = o+1, k4+k, hk, h4+h, gk, gh, g3+h3+k3+1,
          fk, f4+f, eh, ef, f3h3+e3k3+e3+f3+h3+k3+1,
          e3g+f3g+g, e4+e, dh3+dk3+d, dg, df, de,
          d3+e3+f3+1, e2g2+d2h2+c, f2g2+d2k2+b,
          f2h2+e2k2+a;
assPrimes(I,1);
assPrimes(I,2);
assPrimes(I,3);
assPrimes(I,3,4);

tst_status(1);$
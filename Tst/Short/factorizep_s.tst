LIB "tst.lib";
tst_init();

ring r= 31991,(z,w),dp;
poly f= 10582*w^3*z^4+21325*z^4+29620*w^4*z^3+23697*w*z^3+12439*w^6*z^2+3572*w^3*z^2+5463*z^2+16590*w*z+24885*w^3+31963;

factorize (f);

tst_status(1);$

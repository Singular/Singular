LIB "tst.lib";
tst_init();

ring rng = 0,(x,y),dp;
ideal I = x^32767;
ring G = 0,(a,b,c,d),dp;
map m = rng,a^32767,b;
m(I);

tst_status(1); $

LIB "tst.lib";
tst_init();

LIB"alexpoly.lib";

ring rng = 0,(x,y,z),dp;
poly f=(y4-2x3y2-4x5y+x6-x7)*(y2-x3);
semigroup(f);

tst_status(1); $;


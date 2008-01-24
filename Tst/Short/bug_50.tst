LIB "tst.lib";
tst_init();

// tried to reduce a poly with 0

LIB"ncall.lib";
ring r=0,(a,b,c,d),dp;
def nc=SuperCommutative(3,4);
setring nc;
ideal I = ab,ca2-db2,ca-db,a3d-cd;
ideal G = twostd(I);
G;
G = twostd(I);
G;

tst_status(1);$

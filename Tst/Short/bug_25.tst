LIB "tst.lib";
tst_init();

LIB "primdec.lib";
ring r=2,(x,y),lp;
poly f=x3+x2y3+xy6+y9+y8+y2;
ideal I=f,jacob(f);
radical(I);

tst_status(1);$

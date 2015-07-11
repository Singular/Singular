LIB "tst.lib";
tst_init();

LIB "primdec.lib";
ring rng = (0,vv),x,ls;
minpoly = (vv^2+1);
ideal I = x;
radical(I);

ring r = (0),(x,y),dp;
ideal I = 5/3*x*y;
radical(I, "KL");

tst_status(1);$

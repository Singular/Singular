LIB "tst.lib";
tst_init();

LIB "primdec.lib";

// check that wrong range[2] in ideal/module is fixed
ring rng1 = (11,vv),(x,y),lp;
minpoly = (vv^2+1);
ideal I = (vv),y;
radical(I,"SL");

// check failing ASSUME in algerad()  is fixed
ring rng2 = (0,vv),x,ls;
minpoly = (vv^2+1);
ideal I = x;
radical(I);

//check undefined quotring bug is fixed
ring rng3 = (0),(x,y),dp;
short = 0 ;
ideal I = 5/3*x*y;
radical(I, "KL");


tst_status(1);$

LIB "tst.lib";
tst_init();

LIB "primdec.lib";
ring r;
poly chi=x2-y2;
ideal I=chi,jacob(chi);
primdecSY(I);

tst_status(1);$

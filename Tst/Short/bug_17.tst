LIB "tst.lib";
tst_init();

LIB "primdec.lib";
ring R=2,(x,y,z),lp;
ideal i=z2+z+1,y2+y+1,x+y;
primdecGTZ(i);
primdecSY(i);

tst_status(1);$

LIB "tst.lib";
tst_init();

LIB "primdec.lib";
ring R=3,(x,y),lp;
ideal i=y4-x3-x,x9-x,y9-y;
list l=primdecGTZ(i);
l;
// Vergleiche mit:
list L=primdecSY(i);
L;

tst_status(1);$

LIB "tst.lib";
tst_init();

ring r=0,(x,y,z),ds;
degBound=4;
option(prot,redSB);
std(ideal(x-x2+y3,z-x));

tst_status(1);$

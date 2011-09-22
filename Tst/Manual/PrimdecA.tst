LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring r=0,(x,y,z),dp;
module N=x*gen(1)+ y*gen(2),
x*gen(1)-x2*gen(2);
list l=PrimdecA(N);
l;
tst_status(1);$

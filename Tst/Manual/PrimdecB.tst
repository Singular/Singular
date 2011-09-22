LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring r=0,(x,y,z),dp;
module N=y*gen(1),y2*gen(2),yz*gen(2),yx*gen(2);
ideal p=y;
list l=PrimdecB(N,p);
l;
tst_status(1);$

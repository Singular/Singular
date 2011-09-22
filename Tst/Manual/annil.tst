LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring r=0,(x,y,z),dp;
module N=x*gen(1), y*gen(2);
ideal ann=annil(N);
ann;
tst_status(1);$
